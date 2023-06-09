/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReTaquitoTypes
open ReTaquitoContracts
open ReTaquito
open ReTaquitoUtils

type Errors.t +=
  | NoTzip16Metadata(PublicKeyHash.t)
  | NoTzip12Metadata(PublicKeyHash.t)
  | TokenIdNotFound(PublicKeyHash.t, int)
  | IllformedToken(PublicKeyHash.t, int, string)

let toolkit = (network: Network.t) => {
  let toolkit = Toolkit.create(network.endpoint)
  toolkit->Toolkit.addExtension(ReTaquitoContracts.Extension.tzip12Module())
  toolkit->Toolkit.addExtension(ReTaquitoContracts.Extension.tzip16Module())

  toolkit
}

module Tzip16 = {
  open Tzip16

  let makeContract = (toolkit, address) => {
    toolkit->Toolkit.addExtension(Extension.tzip16Module())

    toolkit.Toolkit.contract->Tzip16Contract.at(address)->ReTaquitoError.fromPromiseParsed
  }

  let read = contract =>
    (contract->ReTaquitoTypes.Tzip16Contract.tzip16).getMetadata(.)
    ->ReTaquitoError.fromPromiseParsed
    ->Promise.mapError(x =>
      switch x {
      | ReTaquitoError.NoMetadata =>
        NoTzip16Metadata(contract->ReTaquitoTypes.Tzip16Contract.address)
      | e => e
      }
    )
}

module Tzip12 = {
  open Tzip12

  /* Read TZIP12 Metadata directly from the storage, should be used only if
   Taquito's embedded API cannot read them. */
  module Storage = {
    let read = contract =>
      contract->ReTaquitoTypes.Tzip12Tzip16Contract.storage->ReTaquitoError.fromPromiseParsed

    module Decode = {
      let getOptString = (storage, f) =>
        storage.Tzip12Storage.Fields.get(. f)->Option.map(bytes2Char)

      let getOptArray = (storage, f, decoder) =>
        storage.Tzip12Storage.Fields.get(. f)->Option.flatMap(v =>
          v
          ->bytes2Char
          ->JsonEx.parse
          ->Result.flatMap(json =>
            JsonEx.decode(
              json,
              {
                open Json.Decode
                array(decoder)
              },
            )
          )
          ->ResultEx.toOption
        )

      let getOptArrayString = (storage, f) => getOptArray(storage, f, Json.Decode.string)

      let getString = (storage, f, ~onError) => storage->getOptString(f)->onError(f)

      let getOptPkh = (storage, f) =>
        storage->getOptString(f)->Option.flatMap(s => s->PublicKeyHash.build->ResultEx.toOption)

      let getOptInt = (storage, f) =>
        storage.Tzip12Storage.Fields.get(. f)->Option.flatMap(v => v->bytes2Char->int_of_string_opt)

      let getInt = (storage, f, ~onError) => storage->getOptInt(f)->onError(f)

      let getOptBool = (storage, f) =>
        storage.Tzip12Storage.Fields.get(. f)->Option.flatMap(v =>
          v->bytes2Char->bool_of_string_opt
        )
    }

    /* Parse the `token_info` field */
    let parseTokenInfo = (address: PublicKeyHash.t, token_id, token_info) => {
      open Decode
      let onError = (res, fieldName) =>
        res->Result.fromOption(IllformedToken(address, token_id, fieldName))

      // Symbol and name are optional, we make them mandatory only for fungible tokens
      let onSymbolError = (isNFT, res, fieldName) =>
        isNFT ? res->Option.default(Token.defaultSymbol(address))->Ok : onError(res, fieldName)

      // Symbol and name are optional, we make them mandatory only for fungible tokens
      let onNameError = (isNFT, res, fieldName) =>
        isNFT
          ? res->Option.default(Token.defaultName(address, token_id))->Ok
          : onError(res, fieldName)

      let decimals = token_info->getInt("decimals", ~onError)

      let description = token_info->getOptString("description")
      let minter = token_info->getOptPkh("minter")
      let creators = token_info->getOptArrayString("creators")
      let contributors = token_info->getOptArrayString("contributors")
      let publishers = token_info->getOptArrayString("publishers")
      let date = token_info->getOptString("date")
      let blocklevel = token_info->getOptInt("blocklevel")
      let type_ = token_info->getOptString("type")
      let tags = token_info->getOptArrayString("tags")
      let genres = token_info->getOptArrayString("genres")
      let language = token_info->getOptString("language")
      let identifier = token_info->getOptString("identifier")
      let rights = token_info->getOptString("rights")
      let rightUri = token_info->getOptString("rightUri")
      let artifactUri = token_info->getOptString("artifactUri")
      let displayUri = token_info->getOptString("displayUri")
      let thumbnailUri = token_info->getOptString("thumbnailUri")
      let isTransferable = token_info->getOptBool("isBooleanAmount")
      let isBooleanAmount = token_info->getOptBool("isBooleanAmount")
      let shouldPreferSymbol = token_info->getOptBool("shouldPreferSymbol")
      let formats = token_info->getOptArray("formats", Metadata.Decode.formatDecoder)
      let attributes = token_info->getOptArray("attributes", Metadata.Decode.attributeDecoder)
      let isNFT = artifactUri != None || (displayUri != None || isBooleanAmount != Some(true))

      let name = token_info->getString("name", ~onError=onNameError(isNFT))
      let symbol = token_info->getString("symbol", ~onError=onSymbolError(isNFT))

      Result.flatMap2(name, symbol, (name, symbol) =>
        decimals->Result.map(decimals => {
          token_id: token_id,
          name: name,
          decimals: decimals,
          symbol: symbol,
          description: description,
          minter: minter,
          creators: creators,
          contributors: contributors,
          publishers: publishers,
          date: date,
          blocklevel: blocklevel,
          type_: type_,
          tags: tags,
          genres: genres,
          language: language,
          identifier: identifier,
          rights: rights,
          rightUri: rightUri,
          artifactUri: artifactUri,
          displayUri: displayUri,
          thumbnailUri: thumbnailUri,
          isTransferable: isTransferable,
          isBooleanAmount: isBooleanAmount,
          shouldPreferSymbol: shouldPreferSymbol,
          formats: formats,
          attributes: attributes,
        })
      )
    }

    /* Parse a value of the `token_metadata` big map */
    let parseMetadata = (address, tokenId, token) => {
      let illformed = (res, fieldName) =>
        res->Result.fromOption(IllformedToken(address, tokenId, fieldName))

      let token_id =
        token.Tzip12Storage.token_id->Option.map(ReBigNumber.toInt)->illformed("token_id")

      let token_info = token.Tzip12Storage.token_info->illformed("token_info")

      Result.flatMap2(token_id, token_info, (token_id, token_info) =>
        parseTokenInfo(address, token_id, token_info)
      )
    }

    /* A token_metadata value is illformed if its components are not annotated,
     hence their field in the Taquito generated object is undefined. */
    let isIllformed = metadata =>
      metadata.Tzip12Storage.token_id->Option.isNone || metadata.token_info->Option.isNone

    let fromUnannotated = unt => {
      open Tzip12Storage
      {
        token_id: unt.un_token_id,
        token_info: unt.un_token_info,
      }
    }

    /* Read the `token_metadata` big map, generates the metadata from the
       annotated value, and fallback to the unannotated one if it is not
       available */
    let elaborateFromTokenMetadata = (address, tokenId, metadataMap: Tzip12Storage.Tokens.t<_>) => {
      let key = tokenId->Int64.of_int->BigNumber.fromInt64

      let metadata = metadataMap.get(. key)->ReTaquitoError.fromPromiseParsed

      let metadata = metadata->Promise.flatMapOk(metadata => {
        let isIllformed = metadata->Option.mapWithDefault(false, isIllformed)

        isIllformed
          ? metadataMap
            ->Tzip12Storage.Tokens.getUnannotated(key)
            ->ReTaquitoError.fromPromiseParsed
            ->Promise.mapOk(m => m->Option.map(fromUnannotated))
          : Promise.ok(metadata)
      })

      metadata->Promise.flatMapOk(metadata =>
        metadata->Result.fromOption(TokenIdNotFound(address, tokenId))->Promise.value
      )
    }

    /* Retrieve a token from the storage */
    let getToken = (address, storage, tokenId) => {
      let getTokenMetadata = storage =>
        storage.Tzip12Storage.token_metadata
        ->Result.fromOption(NoTzip12Metadata(address))
        ->Promise.value

      let metadataMap = getTokenMetadata(storage)

      let metadataMap = Promise.flatMap(metadataMap, metadataMap =>
        switch metadataMap {
        | Ok(m) => Promise.ok(m)
        | Error(e) => storage.assets->Option.mapWithDefault(Promise.err(e), getTokenMetadata)
        }
      )

      let metadata =
        metadataMap->Promise.flatMapOk(metadataMap =>
          elaborateFromTokenMetadata(address, tokenId, metadataMap)
        )

      metadata->Promise.flatMapOk(metadata =>
        parseMetadata(address, tokenId, metadata)->Promise.value
      )
    }
  }

  let makeContract = (toolkit, address) => {
    toolkit->Toolkit.addExtension(Extension.tzip12Module())
    toolkit->Toolkit.addExtension(Extension.tzip16Module())

    toolkit.Toolkit.contract->Tzip12Tzip16Contract.at(address)->ReTaquitoError.fromPromiseParsed
  }

  let readFromStorage = (contract, tokenId) =>
    Storage.read(contract)->Promise.flatMapOk(storage =>
      Storage.getToken(contract->ReTaquitoTypes.Tzip12Tzip16Contract.address, storage, tokenId)
    )

  let read = (contract, tokenId) => {
    let metadata =
      (contract->ReTaquitoTypes.Tzip12Tzip16Contract.tzip12).getTokenMetadata(. tokenId)
      ->ReTaquitoError.fromPromiseParsed
      ->Promise.mapError(x =>
        switch x {
        | ReTaquitoError.TokenIdNotFound =>
          TokenIdNotFound(contract->ReTaquitoTypes.Tzip12Tzip16Contract.address, tokenId)
        | ReTaquitoError.NoTokenMetadata =>
          NoTzip12Metadata(contract->ReTaquitoTypes.Tzip12Tzip16Contract.address)
        | e => e
        }
      )

    metadata->Promise.flatMap(x =>
      switch x {
      | Error(NoTzip12Metadata(_) | TokenIdNotFound(_)) => readFromStorage(contract, tokenId)
      | r => Promise.value(r)
      }
    )
  }

  let readContractMetadata = (contract: Tzip12Tzip16Contract.t) =>
    (contract->ReTaquitoTypes.Tzip12Tzip16Contract.tzip16).getMetadata(.)
    ->ReTaquitoError.fromPromiseParsed
    ->Promise.mapError(x =>
      switch x {
      | ReTaquitoError.NoMetadata =>
        NoTzip16Metadata(contract->ReTaquitoTypes.Tzip12Tzip16Contract.address)
      | e => e
      }
    )
}

let () = Errors.registerHandler("MetadataAPI", x =>
  switch x {
  | NoTzip16Metadata(pkh) => I18n.Form_input_error.no_metadata(Some((pkh :> string)))->Some
  | NoTzip12Metadata(pkh) => I18n.Form_input_error.no_token_metadata(Some((pkh :> string)))->Some
  | TokenIdNotFound(pkh, tokenId) =>
    I18n.Form_input_error.token_id_not_found(Some(((pkh :> string), tokenId)))->Some
  | IllformedToken(pkh, tokenId, field) =>
    I18n.Form_input_error.illformed_token_metadata((pkh :> string), tokenId, field)->Some
  | _ => None
  }
)
