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

module Path = {
  module Endpoint = {
    let delegates = "/chains/main/blocks/head/context/delegates\\?active=true";
  };
  module API = {
    let mempool_operations = "mempool/accounts";
  };
};

module URL = {
  type t = string;

  let arg_opt = (v, n, f) => v->Option.map(a => (n, f(a)));

  let build_args = l =>
    l->List.map(((a, v)) => a ++ "=" ++ v)->List.toArray
    |> Js.Array.joinWith("&");

  let build_explorer_url = (network, path, args) => {
    AppSettings.explorer(network)
    ++ "/"
    ++ path
    ++ (args == [] ? "" : "?" ++ args->build_args);
  };

  let fromString = s => s;

  let get = url =>
    url
    ->Fetch.fetch
    ->FutureJs.fromPromise(Js.String.make)
    ->Future.flatMapOk(response =>
        response->Fetch.Response.json->FutureJs.fromPromise(Js.String.make)
      );

  module Explorer = {
    let operations =
        (
          settings: AppSettings.t,
          account,
          ~types: option(array(string))=?,
          ~destination: option(string)=?,
          ~limit: option(int)=?,
          (),
        ) => {
      let operationsPath = "accounts/" ++ account ++ "/operations";
      let args =
        Lib.List.(
          []
          ->addOpt(destination->arg_opt("destination", dst => dst))
          ->addOpt(limit->arg_opt("limit", lim => lim->Js.Int.toString))
          ->addOpt(types->arg_opt("types", t => t->Js.Array2.joinWith(",")))
        );
      let url = build_explorer_url(settings, operationsPath, args);
      url;
    };

    let mempool = (network, ~account) => {
      let path =
        Path.API.mempool_operations ++ "/" ++ account ++ "/operations";
      build_explorer_url(network, path, []);
    };

    let checkToken = (network, ~contract) => {
      let path = "tokens/exists/" ++ contract;
      build_explorer_url(network, path, []);
    };

    let getTokenBalance = (network, ~contract, ~account) => {
      let path =
        "accounts/" ++ account ++ "/tokens/" ++ contract ++ "/balance";
      build_explorer_url(network, path, []);
    };
  };

  module Endpoint = {
    let delegates = settings =>
      AppSettings.endpoint(settings) ++ Path.Endpoint.delegates;
  };

  module External = {
    let bakingBadBakers = "https://api.baking-bad.org/v2/bakers";
  };
};

let tryMap = (result: Result.t('a, string), transform: 'a => 'b) =>
  try(
    switch (result) {
    | Ok(value) => Ok(transform(value))
    | Error(error) => Error(error)
    }
  ) {
  | Json.ParseError(error) => Error(error)
  | Json.Decode.DecodeError(error) => Error(error)
  | _ => Error("Unknown error")
  };

module type Explorer = {
  let getFromMempool:
    (string, AppSettings.t, array(Operation.Read.t)) =>
    Future.t(Result.t(array(Operation.Read.t), string));

  let get:
    (
      AppSettings.t,
      string,
      ~types: array(string)=?,
      ~destination: string=?,
      ~limit: int=?,
      ~mempool: bool=?,
      unit
    ) =>
    Future.t(Result.t(array(Operation.Read.t), string));
};

module ExplorerMaker =
       (Get: {let get: string => Future.t(Result.t(Js.Json.t, string));}) => {
  let getFromMempool = (account, network, operations) =>
    network
    ->URL.Explorer.mempool(~account)
    ->Get.get
    ->Future.map(result =>
        result->tryMap(x =>
          (
            operations,
            x |> Json.Decode.(array(Operation.Read.decodeFromMempool)),
          )
        )
      )
    >>= (
      ((operations, mempool)) => {
        module Comparator = Operation.Read.Comparator;
        let operations =
          Set.fromArray(operations, ~id=(module Operation.Read.Comparator));

        let operations =
          mempool->Array.reduce(operations, Set.add)->Set.toArray;

        Future.value(Ok(operations));
      }
    );

  let get =
      (
        network,
        account,
        ~types: option(array(string))=?,
        ~destination: option(string)=?,
        ~limit: option(int)=?,
        ~mempool: bool=false,
        (),
      ) =>
    network
    ->URL.Explorer.operations(account, ~types?, ~destination?, ~limit?, ())
    ->Get.get
    ->Future.map(result =>
        result->tryMap(Json.Decode.(array(Operation.Read.decode)))
      )
    >>= (
      operations =>
        mempool
          ? getFromMempool(account, network, operations)
          : Future.value(Ok(operations))
    );
};

module Explorer = ExplorerMaker(URL);
