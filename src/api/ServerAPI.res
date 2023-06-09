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

module Path = {
  module Endpoint = {
    let runView = "/chains/main/blocks/head/helpers/scripts/run_view"
  }
}

type Errors.t +=
  | UnknownNetwork(Network.chainId)
  | FetchError(string)
  | JsonResponseError(string)
  | JsonError(string)

let () = Errors.registerHandler("Server", x =>
  switch x {
  | UnknownNetwork(c) => I18n.Errors.unknown_network((c :> string))->Some
  | FetchError(s) => s->Some
  | JsonResponseError(s) => s->Some
  | JsonError(s) => s->Some
  | _ => None
  }
)

@module("uuid") external genUuid: unit => string = "v4"

module URL = {
  type t = string

  let arg_opt = (v, n, f) => v->Option.map(a => (n, f(a)))

  let build_args = l =>
    l->List.map(((a, v)) => a ++ ("=" ++ v))->List.toArray |> Js.Array.joinWith("&")

  let build_url = (path, args) => path ++ (args == list{} ? "" : "?" ++ args->build_args)

  let fromString = s => s

  let get = {
    let umamiInstallationId = switch LocalStorage.UmamiInstallationId.get() {
    | Ok(v) => v
    | Error(_) =>
      let id = genUuid()
      LocalStorage.UmamiInstallationId.set(id)
      Js.log("Storage umami installation id not found, generated " ++ id)
      id
    }

    url => {
      let init = Fetch.RequestInit.make(
        ~method_=Fetch.Get,
        ~headers=Fetch.HeadersInit.make({
          "Umami-Version": System.getVersion(),
          "UmamiInstallationHash": umamiInstallationId,
        }),
        (),
      )

      url
      ->Fetch.fetchWithInit(init)
      ->Promise.fromJs(e => (e->RawJsError.fromPromiseError).message->FetchError)
      ->Promise.flatMapOk(response =>
        response
        ->Fetch.Response.json
        ->Promise.fromJs(e => (e->RawJsError.fromPromiseError).message->FetchError)
      )
    }
  }

  let postJson = (url, json) => {
    let init = Fetch.RequestInit.make(
      ~method_=Fetch.Post,
      ~body=Fetch.BodyInit.make(Js.Json.stringify(json)),
      ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
      (),
    )

    url
    ->Fetch.fetchWithInit(init)
    ->Promise.fromJs(e => (e->RawJsError.fromPromiseError).message->FetchError)
    ->Promise.flatMapOk(response =>
      response
      ->Fetch.Response.json
      ->Promise.fromJs(e => (e->RawJsError.fromPromiseError).message->FetchError)
    )
  }

  module Explorer = {
    module Tzkt = {
      let baseURL = (network: Network.network) => {
        switch network.Network.chain {
        | #Mainnet => "https://api.mainnet.tzkt.io/v1/"->Some
        | #Ghostnet => "https://api.ghostnet.tzkt.io/v1/"->Some
        | #Limanet => "https://api.limanet.tzkt.io/v1/"->Some
        | _ => None
        }
      }

      let operations = (
        network: Network.network,
        account: PublicKeyHash.t,
        ~types: option<array<string>>=?,
        ~destination: option<PublicKeyHash.t>=?,
        ~limit: option<int>=?,
        (),
      ) => {
        baseURL(network)->Option.map(baseURL => {
          let args = {
            open List.Infix
            \"@?"(
              types->arg_opt("type", t => t->Js.Array2.joinWith(",")),
              \"@?"(
                limit->arg_opt("limit", lim => lim->Js.Int.toString),
                \"@?"(destination->arg_opt("target", dst => (dst :> string)), list{}),
              ),
            )
          }

          build_url(baseURL ++ "accounts/" ++ (account :> string) ++ "/operations", args)
        })
      }

      let accountExistsUrl = (network: Network.t, ~account: PublicKeyHash.t) => {
        let baseUrl = network->baseURL->Option.getExn
        baseUrl ++ "accounts/" ++ (account :> string)
      }

      let balancesUrl = (network: Network.t, ~addresses: list<PublicKeyHash.t>) => {
        let baseUrl = network->baseURL->Option.getExn
        baseUrl ++
        "accounts?address.in=" ++
        addresses->List.map(a => (a :> string))->List.toArray->Js.Array2.joinWith(",")
      }

      let checkTokenUrl = (network: Network.t, ~contract: PublicKeyHash.t) => {
        let baseUrl = network->baseURL->Option.getExn
        baseUrl ++ "tokens?contract=" ++ (contract :> string)
      }

      let tokenRegistryUrl = (
        network: Network.t,
        ~accounts: list<PublicKeyHash.t>
      ) => {
        let baseUrl = network->baseURL->Option.getExn
        let accountsParam = switch accounts {
        | list{} => ""
        | _ => "&account.in=" ++ accounts->List.toArray->Js.Array2.map(a => (a :> string))->Js.Array2.joinWith(",")
        }
        baseUrl ++ "tokens/balances" ++ "?limit=1000" ++ accountsParam
      }

      let multisigsUrl = (network: Network.t, ~contract: PublicKeyHash.t) => {
        let baseUrl = network->baseURL->Option.getExn
        baseUrl ++ "contracts/" ++ (contract :> string) ++ "/same?includeStorage=true&limit=1000"
      }

      let blocksCountUrl = (network: Network.t) => {
        let baseUrl = network->baseURL->Option.getExn
        baseUrl ++ "blocks/count"
      }
    }
  }

  module Endpoint = {
    let runView = (network: Network.network) => network.endpoint ++ Path.Endpoint.runView

    /* Generates a valid JSON for the run_view RPC */
    let fa12GetBalanceInput = (
      ~network: Network.network,
      ~contract: PublicKeyHash.t,
      ~account: PublicKeyHash.t,
    ) => {
      open Json.Encode
      object_(list{
        ("contract", string((contract :> string))),
        ("entrypoint", string("getBalance")),
        ("chain_id", Network.Encode.chainIdEncoder(network.chain->Network.getChainId)),
        ("input", object_(list{("string", string((account :> string)))})),
        ("unparsing_mode", string("Readable")),
      })
    }

    /*
       Generates a valid JSON for the run_view RPC.

       Example of an expected input for run_view of `balance_of` on an FA2
       contract, for a single address:
       { "contract": "KT1Wx7pXgstiZCas5SvFFUEmBZbnAoacSCxo",
         "entrypoint": "balance_of",
         "input":
           [ { "prim": "Pair",
               "args":
                 [ { "bytes": "0000721765c758aacce0986e781ddc9a40f5b6b9d9c3" },
                   { "int": "0" } ] } ],
         "chain_id": "NetXz969SFaFn8k",
         "source": "tz1W3HkgNtCvZkLcxPbLpR9mf8vuw4k3atvB",
         "unparsing_mode": "Readable" }

       In the input, `bytes` is the encoded address as bytes, but the
       Michelson typer is also able to type `string` as address. The input of
       the entrypoint is actually a Michelson list of `(pkh * tokenId)`: the
       entrypoint can retrieve the balance for multiple address at once. This
       version only calls the contract for one address.
 */

    let fa2BalanceOfInput = (
      ~network: Network.network,
      ~contract: PublicKeyHash.t,
      ~account: PublicKeyHash.t,
      ~tokenId: int,
    ) => {
      open Json.Encode
      object_(list{
        ("contract", string((contract :> string))),
        ("entrypoint", string("balance_of")),
        ("chain_id", Network.Encode.chainIdEncoder(network.chain->Network.getChainId)),
        (
          "input",
          jsonArray([
            object_(list{
              ("prim", string("Pair")),
              (
                "args",
                jsonArray([
                  object_(list{("string", string((account :> string)))}),
                  object_(list{("int", string(string_of_int(tokenId)))}),
                ]),
              ),
            }),
          ]),
        ),
        ("source", string((account :> string))),
        ("unparsing_mode", string("Readable")),
      })
    }
  }

  module External = {
    let bakingBadBakers = "https://api.baking-bad.org/v2/bakers"

    let tzktAccountTokens = (
      ~network: Network.network,
      ~account: PublicKeyHash.t,
      ~contract: option<PublicKeyHash.t>=?,
      ~limit: option<int>=?,
      ~index: option<int>=?,
      ~hideEmpty: option<bool>=?,
      ~sortBy: option<[#Id | #Balance | #Contract]>=?,
      (),
    ) => {
      let balanceMin = hideEmpty == Some(true) ? Some(0) : None
      let args = {
        open List.Infix
        \"@:"(
          ("account", (account :> string)),
          \"@?"(
            balanceMin->arg_opt("balance.gt", Js.Int.toString),
            \"@?"(
              sortBy->arg_opt("sort.asc", x =>
                switch x {
                | #Id => "id"
                | #Balance => "balance"
                | #Contract => "token.contract"
                }
              ),
              \"@?"(
                index->arg_opt("offset", Js.Int.toString),
                \"@?"(
                  limit->arg_opt("limit", Js.Int.toString),
                  \"@?"(contract->arg_opt("token.contract", k => (k :> string)), list{}),
                ),
              ),
            ),
          ),
        )
      }
      network.chain
      ->Network.chainNetwork
      ->Option.map(network =>
        build_url("https://api." ++ (network ++ ".tzkt.io/v1/tokens/balances/"), args)
      )
      ->ResultEx.fromOption(UnknownNetwork(Network.getChainId(network.chain)))
    }

    let tzktAccountTokensNumber = (~network: Network.network, ~account: PublicKeyHash.t) =>
      network.chain
      ->Network.chainNetwork
      ->Option.map(network =>
        build_url(
          "https://api." ++ (network ++ ".tzkt.io/v1/tokens/balances/count"),
          list{("account", (account :> string))},
        )
      )
      ->ResultEx.fromOption(UnknownNetwork(Network.getChainId(network.chain)))

    let tzktContractStorage = (~network: Network.network, ~contract: PublicKeyHash.t) =>
      network.chain
      ->Network.chainNetwork
      ->Option.map(network =>
        build_url(
          "https://api." ++
          (network ++
          ".tzkt.io/v1/contracts/" ++
          (contract :> string) ++
          "/storage"),
          list{},
        )
      )
      ->ResultEx.fromOption(UnknownNetwork(Network.getChainId(network.chain)))

    let tzktBigmapKeys = (~network: Network.network, ~bigmap: int) =>
      network.chain
      ->Network.chainNetwork
      ->Option.map(network =>
        build_url(
          "https://api." ++ (network ++ ".tzkt.io/v1/bigmaps/" ++ bigmap->Int.toString ++ "/keys"),
          list{},
        )
      )
      ->ResultEx.fromOption(UnknownNetwork(Network.getChainId(network.chain)))
  }
}

module type Explorer = {
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

module ExplorerMaker = (
  Get: {
    let get: string => Promise.t<Js.Json.t>
  },
) => {
  let isMalformedTokenTransfer = (op: Operation.t) =>
    switch op.payload {
    | Transaction(Tez({destination})) => destination->PublicKeyHash.isContract
    | _ => false
    }

  let filterMalformedDuplicates = ops => {
    open Operation
    let l = ops->List.fromArray->List.sort((o1, o2) => compare((o1.hash, o1.id, o1.internal), (o2.hash, o2.id, o2.internal)))

    let rec loop = (acc, l) =>
      switch l {
      | list{} => acc
      | list{h} => list{h, ...acc}
      | list{h1, h2, ...t} =>
        if h1.hash == h2.hash && h1.id == h2.id && h1.internal == h2.internal {
          let h1 = h1->isMalformedTokenTransfer ? None : Some(h1)
          let h2 = h2->isMalformedTokenTransfer ? None : Some(h2)

          open List.Infix
          loop(\"@?"(h1, acc), \"@?"(h2, t))
        } else {
          loop(list{h1, ...acc}, list{h2, ...t})
        }
      }

    loop(list{}, l)->List.reverse->List.toArray
  }

  module Tzkt = {
    let getOperations = (
      network,
      account: PublicKeyHash.t,
      ~types: option<array<string>>=?,
      ~destination: option<PublicKeyHash.t>=?,
      ~limit: option<int>=?,
      (),
    ) =>
      Option.mapWithDefault(
        URL.Explorer.Tzkt.operations(network, account, ~types?, ~destination?, ~limit?, ()),
        Result.Error(Errors.Generic("Unsupported network"))->Promise.value,
        url =>
          Get.get(url)
          ->Promise.flatMapOk(res =>
            res
            ->Result.fromExn({
              open Json.Decode
              array(Operation.Decode.Tzkt.t)
            })
            ->Result.map(array => Array.keepMap(array, x => x))
            ->Result.mapError(e => e->JsonEx.filterJsonExn->JsonError)
            ->Promise.value
          )
          ->Promise.mapOk(filterMalformedDuplicates),
      )

    let getBalances = (network: Network.t, ~addresses: list<PublicKeyHash.t>): Promise.t<
      array<(PublicKeyHash.t, Tez.t)>,
    > => {
      // TZKT doesn't return values for empty accounts
      let defaultBalances = addresses->List.toArray->Array.map(a => (a, Tez.fromMutezInt(0)))

      network
      ->URL.Explorer.Tzkt.balancesUrl(~addresses)
      ->Get.get
      ->Promise.flatMapOk(res => {
        let decoder = json => {
          open Json.Decode
          (
            json |> field("address", PublicKeyHash.decoder),
            json |> field("balance", Tez.fromIntDecoder)
          )
        }
        res
        ->Result.fromExn({
          open Json.Decode
          array(decoder)
        })
        ->Result.mapError(e => e->JsonEx.filterJsonExn->JsonError)
        ->Promise.value
        ->Promise.mapOk(fetchedBalances => Array.concat(defaultBalances, fetchedBalances))
      })
    }

    type multisigContract = {
      address: PublicKeyHash.t,
      signers: array<PublicKeyHash.t>
    }

    let getMultisigs = (network: Network.t, ~addresses: array<PublicKeyHash.t>, ~contract: PublicKeyHash.t) =>
      network
      ->URL.Explorer.Tzkt.multisigsUrl(~contract)
      ->Get.get
      ->Promise.flatMapOk(response => {
        let decoder = json => {
          open Json.Decode
          {
            address: json |> field("address", PublicKeyHash.decoder),
            signers: json |> field("storage", storageJson => storageJson |> field("signers", array(PublicKeyHash.decoder))),
          }
        }

        let addressesSet = Set.fromArray(addresses, ~id=module(PublicKeyHash.Comparator))
        decoder
        ->Json.Decode.array(response)
        ->Js.Array2.filter(contract => {
            !(addressesSet
            ->Set.intersect(Set.fromArray(contract.signers, ~id=module(PublicKeyHash.Comparator)))
            ->Set.isEmpty)
          })
        ->Js.Array2.map(contract => contract.address)
        ->Promise.ok
      })

    let getBlocksCount = (network: Network.t) =>
      network
      ->URL.Explorer.Tzkt.blocksCountUrl
      ->Get.get
      ->Promise.flatMapOk(json => {
        json
        ->Js.Json.decodeNumber
        ->Option.map(Int.fromFloat)
        ->Option.getExn
        ->Promise.ok
      })
  }
}

module Explorer = ExplorerMaker(URL)
