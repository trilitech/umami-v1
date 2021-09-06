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

open UmamiCommon;
open Let;

module Path = {
  module Endpoint = {
    let delegates = "/chains/main/blocks/head/context/delegates\\?active=true";
    let runView = "/chains/main/blocks/head/helpers/scripts/run_view";
  };
};

type Errors.t +=
  | FetchError(string)
  | JsonResponseError(string)
  | JsonError(string);

let () =
  Errors.registerHandler(
    "Server",
    fun
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

  let build_explorer_url = (network, path, args) => {
    ConfigUtils.explorer(network)
    ++ "/"
    ++ path
    ++ (args == [] ? "" : "?" ++ args->build_args);
  };

  let fromString = s => s;

  let get = url => {
    let%FRes response =
      url
      ->Fetch.fetch
      ->FutureJs.fromPromise(e =>
          e->RawJsError.fromPromiseError.message->FetchError
        );

    response
    ->Fetch.Response.json
    ->FutureJs.fromPromise(e =>
        e->RawJsError.fromPromiseError.message->FetchError
      );
  };

  let postJson = (url, json) => {
    let init =
      Fetch.RequestInit.make(
        ~method_=Fetch.Post,
        ~body=Fetch.BodyInit.make(Js.Json.stringify(json)),
        ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
        (),
      );

    let%FRes response =
      url
      ->Fetch.fetchWithInit(init)
      ->FutureJs.fromPromise(e =>
          e->RawJsError.fromPromiseError.message->FetchError
        );

    response
    ->Fetch.Response.json
    ->FutureJs.fromPromise(e =>
        e->RawJsError.fromPromiseError.message->FetchError
      );
  };

  module Explorer = {
    let operations =
        (
          settings: ConfigFile.t,
          account: PublicKeyHash.t,
          ~types: option(array(string))=?,
          ~destination: option(PublicKeyHash.t)=?,
          ~limit: option(int)=?,
          (),
        ) => {
      let operationsPath =
        "accounts/" ++ (account :> string) ++ "/operations/next";
      let args =
        Lib.List.(
          []
          ->addOpt(
              destination->arg_opt("destination", dst => (dst :> string)),
            )
          ->addOpt(limit->arg_opt("limit", lim => lim->Js.Int.toString))
          ->addOpt(types->arg_opt("types", t => t->Js.Array2.joinWith(",")))
        );
      let url = build_explorer_url(settings, operationsPath, args);
      url;
    };

    let checkToken = (network, ~contract: PublicKeyHash.t) => {
      let path = "tokens/exists/" ++ (contract :> string);
      build_explorer_url(network, path, []);
    };
  };

  module Endpoint = {
    let delegates = settings =>
      ConfigUtils.endpoint(settings) ++ Path.Endpoint.delegates;

    let runView = settings =>
      ConfigUtils.endpoint(settings) ++ Path.Endpoint.runView;

    /* Generates a valid JSON for the run_view RPC */
    let fa12GetBalanceInput =
        (~settings, ~contract: PublicKeyHash.t, ~account: PublicKeyHash.t) => {
      Json.Encode.(
        object_([
          ("contract", string((contract :> string))),
          ("entrypoint", string("getBalance")),
          ("chain_id", string(settings->ConfigUtils.chainId)),
          ("input", object_([("string", string((account :> string)))])),
          ("unparsing_mode", string("Readable")),
        ])
      );
    };
  };

  module External = {
    let bakingBadBakers = "https://api.baking-bad.org/v2/bakers";
  };
};

module type Explorer = {
  let getOperations:
    (
      ConfigFile.t,
      PublicKeyHash.t,
      ~types: array(string)=?,
      ~destination: PublicKeyHash.t=?,
      ~limit: int=?,
      unit
    ) =>
    Future.t(Result.t(array(Operation.Read.t), Errors.t));
};

module ExplorerMaker =
       (Get: {let get: string => Future.t(Result.t(Js.Json.t, Errors.t));}) => {
  let getOperations =
      (
        network,
        account: PublicKeyHash.t,
        ~types: option(array(string))=?,
        ~destination: option(PublicKeyHash.t)=?,
        ~limit: option(int)=?,
        (),
      ) => {
    let%FRes res =
      network
      ->URL.Explorer.operations(account, ~types?, ~destination?, ~limit?, ())
      ->Get.get;

    let%FRes operations =
      res
      ->ResultEx.fromExn(Json.Decode.(array(Operation.Read.Decode.t)))
      ->ResultEx.mapError(e => e->Operation.Read.filterJsonExn->JsonError)
      ->Future.value;

    operations->FutureEx.ok;
  };
};

module Explorer = ExplorerMaker(URL);
