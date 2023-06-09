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

type chainId = private string

let apiLowestBound: Version.t
let apiHighestBound: Version.t
let checkInBound: Version.t => bool

type status =
  | Online
  | Pending
  | Offline

type httpError = [#HttpError(string) | #AbortError]

type jsonError = [#JsonError(string)]

type apiError =
  | VersionFormat(string)
  | NotAvailable([httpError | jsonError])
  | VersionRPCError(string)
  | MonitorRPCError(string)
  | NotSupported(Version.t)

type nodeError =
  | NotAvailable([httpError | jsonError])
  | ChainRPCError(string)
  | VersionRPCError(string)

type Errors.t +=
  | EndpointError
  | UnknownChainId(string)
  | ChainInconsistency(string, string)
  | APIAndNodeError(apiError, nodeError)
  | API(apiError)
  | Node(nodeError)

type nativeChains = [#Mainnet | #Ghostnet]

type supportedChains = [
  | nativeChains
  | #Mumbainet
  | #Limanet
  | #Kathmandunet
  | #Jakartanet
  | #Hangzhounet
  | #Granadanet
  | #Florencenet
  | #Edo2net
]

type chain<'chainId> = [supportedChains | #Custom('chainId)]

type configurableChains<'chainId> = [nativeChains | #Custom('chainId)]

let unsafeChainId: string => chainId
let getChainId: [< chain<chainId>] => chainId
let fromChainId: chainId => [> chain<chainId>]

let nativeChains: list<(nativeChains, chainId)>
let supportedChains: list<(supportedChains, chainId)>

let getDisplayedName: [< chain<chainId>] => string
let externalExplorer: [< chain<chainId>] => Promise.result<string>

type network = {
  name: string,
  chain: chain<chainId>,
  endpoint: string,
}

type t = network

let mk: (~endpoint: string, chain<chainId>) => network

let chainNetwork: chain<'chainId> => option<string>
let networkChain: string => option<chain<'chainId>>

module Encode: {
  let chainIdEncoder: Json.Encode.encoder<chainId>
  let chainEncoder: Json.Encode.encoder<[< chain<chainId>]>
  let chainEncoderString: Json.Encode.encoder<[< chain<string>]>
  let encoder: Json.Encode.encoder<network>
}

module Decode: {
  let nativeChainFromString: string => [> nativeChains]
  let chainFromString: string => [> supportedChains]
  let chainIdDecoder: Json.Decode.decoder<chainId>
  let chainDecoder: (string => ([> #Custom(chainId)] as 'a)) => Json.Decode.decoder<'a>
  let chainDecoderString: (string => ([> #Custom(string)] as 'a)) => Json.Decode.decoder<'a>
  let decoder: Json.Decode.decoder<network>
}

let mainnet: network
let ghostnet: network

let getNetwork: nativeChains => network
let getNetworks: nativeChains => list<network>

let testNetwork: network => Promise.t<ReTaquito.RPCClient.blockHeader>

let findValidEndpoint: nativeChains => Promise.t<network>

let checkConfiguration: (string) => Promise.t<chain<chainId>>
