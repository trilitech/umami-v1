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

type Errors.t +=
  | UnknownNetwork(Network.chainId)
  | FetchError(string)
  | JsonResponseError(string)
  | JsonError(string)

@ocaml.doc(" URL generators to access data from the Node or API. ")
module URL: {
  type t

  let fromString: string => t

  let build_url: (string, list<(string, string)>) => string

  module Explorer: {
    @ocaml.doc(" URL generators for TzKT API that can be used as fallback. ")
    module Tzkt: {
      let operations: (
        Network.t,
        PublicKeyHash.t,
        ~types: array<string>=?,
        ~destination: PublicKeyHash.t=?,
        ~limit: int=?,
        unit,
      ) => option<t>

      let accountExistsUrl: (Network.t, ~account: PublicKeyHash.t) => t
      let balancesUrl: (Network.t, ~addresses: list<PublicKeyHash.t>) => t
      let checkTokenUrl: (Network.t, ~contract: PublicKeyHash.t) => t
      let tokenRegistryUrl: (
        Network.t,
        ~accounts: list<PublicKeyHash.t>,
      ) => t
      let multisigsUrl: (Network.t, ~contract: PublicKeyHash.t) => t
      let blocksCountUrl: Network.t => t
    }
  }

  module Endpoint: {
    let runView: Network.t => t

    let fa12GetBalanceInput: (
      ~network: Network.t,
      ~contract: PublicKeyHash.t,
      ~account: PublicKeyHash.t,
    ) => Js.Json.t

    let fa2BalanceOfInput: (
      ~network: Network.t,
      ~contract: PublicKeyHash.t,
      ~account: PublicKeyHash.t,
      ~tokenId: int,
    ) => Js.Json.t
  }

  module External: {
    let bakingBadBakers: t

    let tzktAccountTokens: (
      ~network: Network.t,
      ~account: PublicKeyHash.t,
      ~contract: PublicKeyHash.t=?,
      ~limit: int=?,
      ~index: int=?,
      ~hideEmpty: bool=?,
      ~sortBy: [#Balance | #Contract | #Id]=?,
      unit,
    ) => Promise.result<t>

    let tzktAccountTokensNumber: (
      ~network: Network.t,
      ~account: PublicKeyHash.t,
    ) => Promise.result<t>

    let tzktContractStorage: (
      ~network: Network.network,
      ~contract: PublicKeyHash.t,
    ) => Promise.result<t>

    let tzktBigmapKeys: (~network: Network.network, ~bigmap: int) => Promise.result<t>
  }

  /* Fetch URL as a JSON. */
  let get: t => Promise.t<Js.Json.t>

  /* Fetch URL with a JSON payload, as a JSON. */
  let postJson: (t, Js.Json.t) => Promise.t<Js.Json.t>
}

@ocaml.doc(" Indexer requests for mempool operations and classical operations. ")
module type Explorer = {
  @ocaml.doc(" Tzkt API ")
  module Tzkt: {
    let getOperations: (
      Network.t,
      PublicKeyHash.t,
      ~types: array<string>=?,
      ~destination: PublicKeyHash.t=?,
      ~limit: int=?,
      unit,
    ) => Promise.t<array<Operation.t>>

    let getBalances: (
      Network.t,
      ~addresses: list<PublicKeyHash.t>,
    ) => Promise.t<array<(PublicKeyHash.t, Tez.t)>>

    let getMultisigs: (
      Network.t,
      ~addresses: array<PublicKeyHash.t>,
      ~contract: PublicKeyHash.t,
    ) => Promise.t<array<PublicKeyHash.t>>

    let getBlocksCount: (Network.t) => Promise.t<int>
  }
}
@ocaml.doc(" This generic version exists only for tests purpose ")
module ExplorerMaker: (
  Get: {
    let get: URL.t => Promise.t<Js.Json.t>
  },
) => Explorer

module Explorer: Explorer
