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

type metadata = {
  name: option<string>,
  symbol: option<string>,
  decimals: option<int>, //default: 0
  description: option<string>,
  artifactUri: option<string>,
  displayUri: option<string>,
  thumbnailUri: option<string>,
  externalUri: option<string>,
  isTransferable: option<bool>, // default: true
  isBooleanAmount: option<bool>, // default: false
  shouldPreferSymbol: option<bool>, //default: false
  formats: option<array<Metadata.format>>,
  creators: option<array<string>>,
  tags: option<array<string>>,
}

type contract = {
  alias: option<string>,
  address: PublicKeyHash.t,
}

type tokenInfo = {
  contract: contract,
  tokenId: ReBigNumber.t,
  standard: TokenContract.kind,
  metadata: option<metadata>,
}

type t = {
  balance: ReBigNumber.t,
  tokenInfo: tokenInfo,
}

type tokens = array<t>

let address = token => token.tokenInfo.contract.address
let tokenId = token => token.tokenInfo.tokenId->ReBigNumber.toInt

let isNFT = t =>
  t.tokenInfo.metadata->Option.mapWithDefault(false, m =>
    m.artifactUri != None || (m.displayUri != None || m.isBooleanAmount == Some(true))
  )

let toTokenAsset = token =>
  token.tokenInfo.metadata->Option.map(metadata => {
    open Metadata
    {
      ...defaultAsset,
      description: metadata.description,
      formats: metadata.formats,
      creators: metadata.creators,
      tags: metadata.tags,
      artifactUri: metadata.artifactUri,
      displayUri: metadata.displayUri,
      thumbnailUri: Metadata.thumbnailUriFromFormat(metadata.thumbnailUri, metadata.formats),
      isTransferable: metadata.isTransferable->Option.getWithDefault(true),
      isBooleanAmount: metadata.isBooleanAmount->Option.getWithDefault(false),
      shouldPreferSymbol: metadata.shouldPreferSymbol->Option.getWithDefault(false),
    }
  })

let metadataFromBuiltinTemplate = (metadata, template: TokenRepr.t) => {
  ...metadata,
  name: metadata.name->Option.keep(template.alias->Some),
  symbol: metadata.symbol->Option.keep(template.symbol->Some),
  decimals: metadata.decimals->Option.keep(template.decimals->Some),
  description: metadata.description->Option.keep(template.asset.description),
  artifactUri: metadata.artifactUri->Option.keep(template.asset.artifactUri),
  displayUri: metadata.displayUri->Option.keep(template.asset.displayUri),
  thumbnailUri: metadata.thumbnailUri->Option.keep(template.asset.thumbnailUri),
  isTransferable: metadata.isTransferable->Option.keep(template.asset.isTransferable->Some),
  isBooleanAmount: metadata.isBooleanAmount->Option.keep(template.asset.isBooleanAmount->Some),
  shouldPreferSymbol: metadata.shouldPreferSymbol->Option.keep(
    template.asset.shouldPreferSymbol->Some,
  ),
}

let toTokenRepr = (chain, {tokenInfo: {contract, metadata, tokenId, standard}} as token) => {
  let kind = TokenContract.toTokenKind(standard, tokenId->ReBigNumber.toInt)
  metadata->Option.flatMap(metadata =>
    switch (metadata.symbol, metadata.name, metadata.decimals, chain) {
    | (Some(symbol), Some(name), Some(decimals), Some(chain)) =>
      {
        open TokenRepr
        {
          kind: kind,
          address: contract.address,
          alias: name,
          symbol: symbol,
          decimals: decimals,
          chain: chain,
          asset: token->toTokenAsset->Option.default(Metadata.defaultAsset),
        }
      }->Some
    | _ => None
    }
  )
}

let fromTokenRepr = (token: TokenRepr.t) => {
  let contract = {address: token.address, alias: None}
  let metadata = {
    name: token.alias->Some,
    symbol: token.symbol->Some,
    decimals: token.decimals->Some, //default: 0
    description: token.asset.description,
    artifactUri: token.asset.artifactUri,
    displayUri: token.asset.displayUri,
    thumbnailUri: token.asset.thumbnailUri,
    externalUri: None,
    isTransferable: token.asset.isTransferable->Some,
    isBooleanAmount: token.asset.isBooleanAmount->Some,
    shouldPreferSymbol: token.asset.shouldPreferSymbol->Some,
    formats: token.asset.formats,
    creators: token.asset.creators,
    tags: token.asset.tags,
  }
  let tokenInfo = {
    tokenId: token->TokenRepr.id->ReBigNumber.fromInt,
    contract: contract,
    standard: token.kind->TokenContract.fromTokenKind,
    metadata: Some(metadata),
  }
  {balance: ReBigNumber.fromInt(0), tokenInfo: tokenInfo}
}

let updateFromBuiltinTemplate = token => {
  let template = BuiltinTokens.findTemplate(
    token.tokenInfo.contract.address,
    token.tokenInfo.tokenId->ReBigNumber.toInt,
    token->toTokenAsset,
  )
  template->Option.mapWithDefault(token, template => {
    ...token,
    tokenInfo: {
      ...token.tokenInfo,
      metadata: token.tokenInfo.metadata->Option.map(metadata =>
        metadataFromBuiltinTemplate(metadata, template)
      ),
    },
  })
}

let requestPageSize = 50

module Decode = {
  open JsonEx.Decode

  let metadataDecoder = json => {
    name: json |> optionalOrNull("name", string),
    symbol: json |> optionalOrNull("symbol", string),
    decimals: json |> optionalOrNull("decimals", either(string |> map(int_of_string), int)),
    //default: 0
    description: json |> optionalOrNull("description", string),
    artifactUri: json |> optionalOrNull("artifactUri", string),
    displayUri: json |> optionalOrNull("displayUri", string),
    thumbnailUri: json |> optionalOrNull("thumbnailUri", string),
    externalUri: json |> optionalOrNull("externalUri", string),
    isTransferable: json |> optionalOrNull("isTransferable", bool), // default: true
    isBooleanAmount: json |> optionalOrNull("isBooleanAmount", bool), // default: false
    shouldPreferSymbol: json |> optionalOrNull("shouldPreferSymbol", bool), //default: false
    creators: json |> optionalOrNull("formats", array(string)),
    tags: json |> optionalOrNull("formats", array(string)),
    formats: json |> optionalOrNull("formats", array(Metadata.Decode.formatDecoder)),
  }

  let standardDecoder = s => s->string->TokenContract.Decode.kindFromString->JsonEx.getExn

  let contractDecoder = json => {
    alias: json |> optionalOrNull("alias", string),
    address: json |> field("address", PublicKeyHash.decoder),
  }

  let tokenInfoDecoder = json => {
    tokenId: json |> field("tokenId", string) |> ReBigNumber.fromString,
    contract: json |> field("contract", contractDecoder),
    standard: json |> field("standard", standardDecoder),
    metadata: json |> optionalOrNull("metadata", metadataDecoder),
  }

  let decoder = json => {
    balance: json |> field("balance", string) |> ReBigNumber.fromString,
    tokenInfo: json |> field("token", tokenInfoDecoder),
  }

  let arrayDecoder = array(decoder)
}

module Encode = {
  open JsonEx.Encode

  let metadataEncoder = t =>
    object_(list{
      ("name", t.name |> nullable(string)),
      ("symbol", t.symbol |> nullable(string)),
      ("decimals", t.decimals |> nullable(int)), //default: 0
      ("description", t.description |> nullable(string)),
      ("artifactUri", t.artifactUri |> nullable(string)),
      ("displayUri", t.displayUri |> nullable(string)),
      ("thumbnailUri", t.thumbnailUri |> nullable(string)),
      ("externalUri", t.externalUri |> nullable(string)),
      ("isTransferable", t.isTransferable |> nullable(bool)), // default: true
      ("isBooleanAmount", t.isBooleanAmount |> nullable(bool)), // default: false
      ("shouldPreferSymbol", t.shouldPreferSymbol |> nullable(bool)), //default: false
    })

  let standardEncoder = TokenContract.Encode.kindEncoder

  let contractEncoder = c =>
    object_(list{
      ("alias", c.alias |> nullable(string)),
      ("address", c.address |> PublicKeyHash.encoder),
    })

  let tokenInfoEncoder = t =>
    object_(list{
      ("tokenId", t.tokenId |> ReBigNumber.toString |> string),
      ("contract", t.contract |> contractEncoder),
      ("standard", t.standard |> standardEncoder |> string),
      ("metadata", t.metadata |> nullable(metadataEncoder)),
    })

  let encoder = t =>
    object_(list{
      ("balance", t.balance |> ReBigNumber.toString |> string),
      ("token", t.tokenInfo |> tokenInfoEncoder),
    })
}

// Migration from BetterCallDev API values
module Migration = {
  type bcd = {
    balance: ReBigNumber.t,
    contract: PublicKeyHash.t,
    token_id: int,
    network: string,
    name: option<string>,
    symbol: option<string>,
    decimals: option<int>, //default: 0
    description: option<string>,
    artifact_uri: option<string>,
    display_uri: option<string>,
    thumbnail_uri: option<string>,
    external_uri: option<string>,
    is_transferable: option<bool>, // default: true
    is_boolean_amount: option<bool>, // default: false
    should_prefer_symbol: option<bool>, //default: false
    formats: option<array<Metadata.format>>,
    creators: option<array<string>>,
    tags: option<array<string>>,
  }

  open JsonEx.Decode

  let bcdDecoder = json => {
    balance: json |> field("balance", string) |> ReBigNumber.fromString,
    contract: json |> field("contract", PublicKeyHash.decoder),
    token_id: json |> field("token_id", int),
    network: json |> field("network", string),
    name: json |> optionalOrNull("name", string),
    symbol: json |> optionalOrNull("symbol", string),
    decimals: json |> optionalOrNull("decimals", int), //default: 0
    description: json |> optionalOrNull("description", string),
    artifact_uri: json |> optionalOrNull("artifact_uri", string),
    display_uri: json |> optionalOrNull("display_uri", string),
    thumbnail_uri: json |> optionalOrNull("thumbnail_uri", string),
    external_uri: json |> optionalOrNull("external_uri", string),
    is_transferable: json |> optionalOrNull("is_transferable", bool), // default: true
    is_boolean_amount: json |> optionalOrNull("is_boolean_amount", bool), // default: false
    should_prefer_symbol: json |> optionalOrNull("should_prefer_symbol", bool), //default: false
    creators: json |> optionalOrNull("formats", array(string)),
    tags: json |> optionalOrNull("formats", array(string)),
    formats: json |> optionalOrNull("formats", array(Metadata.Decode.formatDecoder)),
  }

  let migrate = (tokenContract: TokenContract.t, bcd: bcd) => {
    let contract = {address: bcd.contract, alias: None}
    let metadata = {
      name: bcd.name,
      symbol: bcd.symbol,
      decimals: bcd.decimals,
      description: bcd.description,
      artifactUri: bcd.artifact_uri,
      displayUri: bcd.display_uri,
      thumbnailUri: bcd.thumbnail_uri,
      externalUri: bcd.external_uri,
      isTransferable: bcd.is_transferable,
      isBooleanAmount: bcd.is_boolean_amount,
      shouldPreferSymbol: bcd.should_prefer_symbol,
      formats: bcd.formats,
      creators: bcd.creators,
      tags: bcd.tags,
    }
    let tokenInfo = {
      tokenId: bcd.token_id->ReBigNumber.fromInt,
      contract: contract,
      standard: tokenContract.kind,
      metadata: Some(metadata),
    }
    {balance: bcd.balance, tokenInfo: tokenInfo}
  }
}
