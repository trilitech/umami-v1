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

module Path = {
  module Endpoint = {
    let delegates = "/chains/main/blocks/head/context/delegates\\?active=true";
    let runView = "/chains/main/blocks/head/helpers/scripts/run_view";
  };
};

type Errors.t +=
  | UnknownNetwork(string)
  | FetchError(string)
  | JsonResponseError(string)
  | JsonError(string);

let () =
  Errors.registerHandler(
    "Server",
    fun
    | UnknownNetwork(c) => I18n.Errors.unknown_network(c)->Some
    | FetchError(s) => s->Some
    | JsonResponseError(s) => s->Some
    | JsonError(s) => s->Some
    | _ => None,
  );

module URL = {
  type t = string;

  let arg_opt = (v, n, f) => v->Option.map(a => (n, f(a)));

  let build_args = l =>
    l->List.map(((a, v)) => a ++ "=" ++ v)->List.toArray
    |> Js.Array.joinWith("&");

  let build_url = (path, args) => {
    path ++ (args == [] ? "" : "?" ++ args->build_args);
  };

  let build_explorer_url = (config: ConfigContext.env, path, args) => {
    build_url(config.network.explorer ++ "/" ++ path, args);
  };

  let fromString = s => s;

  let get = url => {
    let init =
      Fetch.RequestInit.make(
        ~method_=Fetch.Get,
        ~headers=
          Fetch.HeadersInit.make({
            "Mezos-Version": Network.apiHighestBound->Version.toString,
            "Umami-Version": System.getVersion(),
          }),
        (),
      );

    let%Await response =
      url
      ->Fetch.fetchWithInit(init)
      ->Promise.fromJs(e =>
          e->RawJsError.fromPromiseError.message->FetchError
        );

    response
    ->Fetch.Response.json
    ->Promise.fromJs(e => e->RawJsError.fromPromiseError.message->FetchError);
  };

  let postJson = (url, json) => {
    let init =
      Fetch.RequestInit.make(
        ~method_=Fetch.Post,
        ~body=Fetch.BodyInit.make(Js.Json.stringify(json)),
        ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
        (),
      );

    let%Await response =
      url
      ->Fetch.fetchWithInit(init)
      ->Promise.fromJs(e =>
          e->RawJsError.fromPromiseError.message->FetchError
        );

    response
    ->Fetch.Response.json
    ->Promise.fromJs(e => e->RawJsError.fromPromiseError.message->FetchError);
  };

  module Explorer = {
    let operations =
        (
          config: ConfigContext.env,
          account: PublicKeyHash.t,
          ~types: option(array(string))=?,
          ~destination: option(PublicKeyHash.t)=?,
          ~limit: option(int)=?,
          (),
        ) => {
      let operationsPath = "accounts/" ++ (account :> string) ++ "/operations";
      let args =
        List.Infix.(
          types->arg_opt("types", t => t->Js.Array2.joinWith(","))
          @? limit->arg_opt("limit", lim => lim->Js.Int.toString)
          @? destination->arg_opt("destination", dst => (dst :> string))
          @? []
        );
      let url = build_explorer_url(config, operationsPath, args);
      url;
    };

    let checkToken = (config, ~contract: PublicKeyHash.t) => {
      let path = "tokens/exists/" ++ (contract :> string);
      build_explorer_url(config, path, []);
    };

    let accountExists = (config, ~account: PublicKeyHash.t) => {
      let path = "accounts/" ++ (account :> string) ++ "/exists/";
      build_explorer_url(config, path, []);
    };

    let tokenRegistry =
        (
          network,
          ~accountsFilter: list(PublicKeyHash.t)=[],
          ~kinds=?,
          ~limit=?,
          ~index=?,
          (),
        ) => {
      let args =
        List.Infix.(
          index->arg_opt("index", Int64.to_string)
          @? limit->arg_opt("limit", Int64.to_string)
          @? kinds->arg_opt("kinds", kinds =>
               List.map(kinds, TokenContract.Encode.kindEncoder)
               |> String.concat(",")
             )
          @? accountsFilter->List.map(a => ("accounts", (a :> string)))
        );
      build_explorer_url(network, "tokens/registry", args);
    };
  };

  module Endpoint = {
    let delegates = (c: ConfigContext.env) =>
      c.network.endpoint ++ Path.Endpoint.delegates;

    let runView = (c: ConfigContext.env) =>
      c.network.endpoint ++ Path.Endpoint.runView;

    /* Generates a valid JSON for the run_view RPC */
    let fa12GetBalanceInput =
        (
          ~config: ConfigContext.env,
          ~contract: PublicKeyHash.t,
          ~account: PublicKeyHash.t,
        ) => {
      Json.Encode.(
        object_([
          ("contract", string((contract :> string))),
          ("entrypoint", string("getBalance")),
          ("chain_id", string(config.network.chain->Network.getChainId)),
          ("input", object_([("string", string((account :> string)))])),
          ("unparsing_mode", string("Readable")),
        ])
      );
    };

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

    let fa2BalanceOfInput =
        (
          ~config: ConfigContext.env,
          ~contract: PublicKeyHash.t,
          ~account: PublicKeyHash.t,
          ~tokenId: int,
        ) => {
      Json.Encode.(
        object_([
          ("contract", string((contract :> string))),
          ("entrypoint", string("balance_of")),
          ("chain_id", string(config.network.chain->Network.getChainId)),
          (
            "input",
            jsonArray([|
              object_([
                ("prim", string("Pair")),
                (
                  "args",
                  jsonArray([|
                    object_([("string", string((account :> string)))]),
                    object_([("int", string(string_of_int(tokenId)))]),
                  |]),
                ),
              ]),
            |]),
          ),
          ("source", string((account :> string))),
          ("unparsing_mode", string("Readable")),
        ])
      );
    };
  };

  module External = {
    let bakingBadBakers = "https://api.baking-bad.org/v2/bakers";

    let betterCallDevAccountTokens =
        (
          ~config: ConfigContext.env,
          ~account: PublicKeyHash.t,
          ~contract: option(PublicKeyHash.t)=?,
          ~limit: option(int)=?,
          ~index: option(int)=?,
          ~hideEmpty: option(bool)=?,
          ~sortBy: option([ | `TokenId | `Balance])=?,
          (),
        ) => {
      let args =
        List.Infix.(
          hideEmpty->arg_opt("hide_empty", string_of_bool)
          @? sortBy->arg_opt(
               "sort_by",
               fun
               | `TokenId => "token_id"
               | `Balance => "balance",
             )
          @? index->arg_opt("offset", Js.Int.toString)
          @? limit->arg_opt("size", Js.Int.toString)
          @? contract->arg_opt("contract", k => (k :> string))
          @? []
        );
      config.network.chain
      ->Network.chainNetwork
      ->Option.map(network =>
          build_url(
            "https://api.better-call.dev/v1/account/"
            ++ network
            ++ "/"
            ++ (account :> string)
            ++ "/token_balances",
            args,
          )
        )
      ->ResultEx.fromOption(
          UnknownNetwork(Network.getChainId(config.network.chain)),
        );
    };

    let betterCallDevBatchAccounts =
        (~config: ConfigContext.env, ~accounts: array(PublicKeyHash.t)) => {
      let args = [
        (
          "address",
          // array(pkh) is not a subtype of array(string), hence we must map it
          accounts->Array.map(a => (a :> string))->Js.Array2.joinWith(","),
        ),
      ];
      config.network.chain
      ->Network.chainNetwork
      ->Option.map(network =>
          build_url(
            "https://api.better-call.dev/v1/account/" ++ network,
            args,
          )
        )
      ->ResultEx.fromOption(
          UnknownNetwork(Network.getChainId(config.network.chain)),
        );
    };
  };
};

module type Explorer = {
  let getOperations:
    (
      ConfigContext.env,
      PublicKeyHash.t,
      ~types: array(string)=?,
      ~destination: PublicKeyHash.t=?,
      ~limit: int=?,
      unit
    ) =>
    Promise.t(array(Operation.Read.t));
};

module ExplorerMaker = (Get: {let get: string => Promise.t(Js.Json.t);}) => {
  let isMalformedTokenTransfer = (op: Operation.Read.t) =>
    switch (op.payload) {
    | Operation.Transaction.(Transaction(Tez({destination}))) =>
      destination->PublicKeyHash.isContract
    | _ => false
    };

  let filterMalformedDuplicates = ops => {
    open Operation.Read;
    let l =
      ops
      ->List.fromArray
      ->List.sort((o1, o2) => compare((o1.hash, o1.id), (o2.hash, o2.id)));

    let rec loop = (acc, l) =>
      switch (l) {
      | [] => acc
      | [h] => [h, ...acc]
      | [h1, h2, ...t] =>
        if (h1.hash == h2.hash && h1.id == h2.id) {
          let h1 = h1->isMalformedTokenTransfer ? None : Some(h1);
          let h2 = h2->isMalformedTokenTransfer ? None : Some(h2);

          List.Infix.(loop(h1 @? acc, h2 @? t));
        } else {
          loop([h1, ...acc], [h2, ...t]);
        }
      };

    loop([], l)->List.reverse->List.toArray;
  };

  let getOperations =
      (
        network,
        account: PublicKeyHash.t,
        ~types: option(array(string))=?,
        ~destination: option(PublicKeyHash.t)=?,
        ~limit: option(int)=?,
        (),
      ) => {
    let%Await res =
      network
      ->URL.Explorer.operations(account, ~types?, ~destination?, ~limit?, ())
      ->Get.get;

    let%AwaitMap operations =
      res
      ->Result.fromExn(Json.Decode.(array(Operation.Read.Decode.t)))
      ->Result.mapError(e => e->Operation.Read.filterJsonExn->JsonError)
      ->Promise.value;
    operations->filterMalformedDuplicates;
  };
};

module Explorer = ExplorerMaker(URL);
