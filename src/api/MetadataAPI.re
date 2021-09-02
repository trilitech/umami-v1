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

open ReTaquitoTypes;
open ReTaquitoContracts;
open ReTaquito;

type Errors.t +=
  | NoTzip16Metadata(PublicKeyHash.t)
  | NoTzip12Metadata(PublicKeyHash.t)
  | TokenIdNotFound(PublicKeyHash.t, int);

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

  let makeContract = (toolkit, address) => {
    toolkit->Toolkit.addExtension(Extension.tzip12Module());

    toolkit.Toolkit.contract
    ->Tzip12Tzip16Contract.at(address)
    ->ReTaquitoError.fromPromiseParsed;
  };

  let read = (contract, tokenId) => {
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
    | _ => None,
  );

module Debug = {
  open Let;
  let init = () => ();

  let fetchTokenMetadata = (address, tokenId) => {
    let toolkit = Toolkit.create("https://granadanet.smartpy.io");
    let%FRes contract = Tzip12.makeContract(toolkit, address);
    Tzip12.read(contract, tokenId);
  };

  let readTokenMetadata = (address, tokenId) => {
    fetchTokenMetadata(address, tokenId)
    ->Future.get(metadata => Js.log(metadata));
  };

  let fetchMetadata = address => {
    let toolkit = Toolkit.create("https://granadanet.smartpy.io");
    let%FRes contract = Tzip16.makeContract(toolkit, address);
    Tzip16.read(contract);
  };

  let readMetadata = address => {
    fetchMetadata(address)->Future.get(metadata => Js.log(metadata));
  };

  type window = {
    mutable readTokenMetadata: (PublicKeyHash.t, int) => unit,
    mutable readMetadata: PublicKeyHash.t => unit,
  };

  [@bs.val] external window: window = "window";

  window.readTokenMetadata = readTokenMetadata;
  window.readMetadata = readMetadata;
};
