/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open Let;

open ReTaquitoTypes;
open ReTaquitoContracts;
open ReTaquito;
open ReTaquitoUtils;

type Errors.t +=
  | NoTzip16Metadata(PublicKeyHash.t)
  | NoTzip12Metadata(PublicKeyHash.t)
  | TokenIdNotFound(PublicKeyHash.t, int)
  | IllformedToken(PublicKeyHash.t, int, string);

module Tzip16 = {
  open Tzip16;

  let makeContract = (toolkit, address) => {
    toolkit->Toolkit.addExtension(Extension.tzip16Module());

    toolkit.Toolkit.contract
    ->Tzip16Contract.at(address)
    ->ReTaquitoError.fromPromiseParsed;
  };

  let read = contract => {
    contract##tzip16().getMetadata(.)
    ->ReTaquitoError.fromPromiseParsed
    ->Future.mapError(
        fun
        | ReTaquitoError.NoMetadata => NoTzip16Metadata(contract##address)
        | e => e,
      );
  };
};

module Tzip12 = {
  open Tzip12;

  /* Read TZIP12 Metadata directly from the storage, should be used only if
     Taquito's embedded API cannot read them. */
  module Storage = {
    let read = contract => {
      contract##storage()->ReTaquitoError.fromPromiseParsed;
    };

    /* Parse the `token_info` field */
    let parseTokenInfo = (address, tokenId, token_info) => {
      let illformed = (res, fieldName) =>
        res->ResultEx.fromOption(
          IllformedToken(address, tokenId, fieldName),
        );

      let%Res name =
        token_info.Tzip12Storage.Fields.get(. "name")
        ->Option.map(bytes2Char)
        ->illformed("name");

      let%Res decimals =
        token_info.get(. "decimals")
        ->Option.flatMap(v => v->bytes2Char->int_of_string_opt)
        ->illformed("decimals");

      let%ResMap symbol =
        token_info.get(. "symbol")
        ->Option.map(bytes2Char)
        ->illformed("symbol");

      (name, decimals, symbol);
    };

    /* Parse a value of the `token_metadata` big map */
    let parseMetadata = (address, tokenId, token) => {
      let illformed = (res, fieldName) =>
        res->ResultEx.fromOption(
          IllformedToken(address, tokenId, fieldName),
        );

      let%Res token_id =
        token.Tzip12Storage.token_id
        ->Option.map(ReBigNumber.toInt)
        ->illformed("token_id");

      let%Res token_info =
        token.Tzip12Storage.token_info->illformed("token_info");
      let%ResMap (name, decimals, symbol) =
        parseTokenInfo(address, tokenId, token_info);

      Tzip12.{token_id, name, symbol, decimals};
    };

    /* A token_metadata value is illformed if its components are not annotated,
       hence their field in the Taquito generated object is undefined. */
    let isIllformed = metadata =>
      metadata.Tzip12Storage.token_id->Option.isNone
      || metadata.token_info->Option.isNone;

    let fromUnannotated = unt => {
      Tzip12Storage.{
        token_id: unt.un_token_id,
        token_info: unt.un_token_info,
      };
    };

    /* Read the `token_metadata` big map, generates the metadata from the
       annotated value, and fallback to the unannotated one if it is not
       available */
    let elaborateFromTokenMetadata =
        (address, tokenId, metadataMap: Tzip12Storage.Tokens.t(_)) => {
      let key = tokenId->Int64.of_int->BigNumber.fromInt64;

      let%FRes metadata =
        metadataMap.get(. key)->ReTaquitoError.fromPromiseParsed;

      let isIllformed = metadata->Option.mapWithDefault(false, isIllformed);

      let%FRes metadata =
        isIllformed
          ? metadataMap
            ->Tzip12Storage.Tokens.getUnannotated(key)
            ->ReTaquitoError.fromPromiseParsed
            ->Future.mapOk(m => m->Option.map(fromUnannotated))
          : FutureEx.ok(metadata);

      metadata
      ->ResultEx.fromOption(TokenIdNotFound(address, tokenId))
      ->Future.value;
    };

    /* Retrieve a token from the storage */
    let getToken = (address, storage, tokenId) => {
      let getTokenMetadata = storage =>
        storage.Tzip12Storage.token_metadata
        ->ResultEx.fromOption(NoTzip12Metadata(address))
        ->Future.value;

      let%Ft metadataMap = getTokenMetadata(storage);

      let%FRes metadataMap =
        switch (metadataMap) {
        | Ok(m) => FutureEx.ok(m)
        | Error(e) =>
          storage.assets
          ->Option.mapWithDefault(FutureEx.err(e), getTokenMetadata)
        };

      let%FRes metadata =
        elaborateFromTokenMetadata(address, tokenId, metadataMap);

      parseMetadata(address, tokenId, metadata)->Future.value;
    };
  };

  let makeContract = (toolkit, address) => {
    toolkit->Toolkit.addExtension(Extension.tzip12Module());

    toolkit.Toolkit.contract
    ->Tzip12Tzip16Contract.at(address)
    ->ReTaquitoError.fromPromiseParsed;
  };

  let readFromStorage = (contract, tokenId) => {
    let%FRes storage = Storage.read(contract);
    Storage.getToken(contract##address, storage, tokenId);
  };

  let read = (contract, tokenId) => {
    let%Ft metadata =
      contract##tzip12().getTokenMetadata(. tokenId)
      ->ReTaquitoError.fromPromiseParsed
      ->Future.mapError(
          fun
          | ReTaquitoError.TokenIdNotFound =>
            TokenIdNotFound(contract##address, tokenId)
          | ReTaquitoError.NoTokenMetadata =>
            NoTzip12Metadata(contract##address)
          | e => e,
        );
    switch (metadata) {
    | Error(NoTzip12Metadata(_) | TokenIdNotFound(_)) =>
      readFromStorage(contract, tokenId)
    | r => Future.value(r)
    };
  };
};

let () =
  Errors.registerHandler(
    "MetadataAPI",
    fun
    | NoTzip16Metadata(pkh) =>
      I18n.form_input_error#no_metadata(Some((pkh :> string)))->Some
    | NoTzip12Metadata(pkh) =>
      I18n.form_input_error#no_token_metadata(Some((pkh :> string)))->Some
    | TokenIdNotFound(pkh, tokenId) =>
      I18n.form_input_error#token_id_not_found(
        Some(((pkh :> string), tokenId)),
      )
      ->Some
    | IllformedToken(pkh, tokenId, field) =>
      I18n.form_input_error#illformed_token_metadata(
        (pkh :> string),
        tokenId,
        field,
      )
      ->Some
    | _ => None,
  );
