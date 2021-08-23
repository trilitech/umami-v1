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
    ConfigUtils.explorer(network)
    ++ "/"
    ++ path
    ++ (args == [] ? "" : "?" ++ args->build_args);
  };

  let fromString = s => s;

  let get = url => {
    let%FRes response =
      url->Fetch.fetch->FutureJs.fromPromise(Js.String.make);

    response->Fetch.Response.json->FutureJs.fromPromise(Js.String.make);
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
      let operationsPath = "accounts/" ++ (account :> string) ++ "/operations";
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

    let mempool = (network, ~account: PublicKeyHash.t) => {
      let path =
        Path.API.mempool_operations
        ++ "/"
        ++ (account :> string)
        ++ "/operations";
      build_explorer_url(network, path, []);
    };

    let checkToken = (network, ~contract: PublicKeyHash.t) => {
      let path = "tokens/exists/" ++ (contract :> string);
      build_explorer_url(network, path, []);
    };

    let getTokenBalance =
        (network, ~contract: PublicKeyHash.t, ~account: PublicKeyHash.t) => {
      let path =
        "accounts/"
        ++ (account :> string)
        ++ "/tokens/"
        ++ (contract :> string)
        ++ "/balance";
      build_explorer_url(network, path, []);
    };
  };

  module Endpoint = {
    let delegates = settings =>
      ConfigUtils.endpoint(settings) ++ Path.Endpoint.delegates;
  };

  module External = {
    let bakingBadBakers = "https://api.baking-bad.org/v2/bakers";
  };
};

module type Explorer = {
  let getFromMempool:
    (PublicKeyHash.t, ConfigFile.t, array(Operation.Read.t)) =>
    Future.t(Result.t(array(Operation.Read.t), string));

  let getOperations:
    (
      ConfigFile.t,
      PublicKeyHash.t,
      ~types: array(string)=?,
      ~destination: PublicKeyHash.t=?,
      ~limit: int=?,
      ~mempool: bool=?,
      unit
    ) =>
    Future.t(Result.t(array(Operation.Read.t), string));
};

module ExplorerMaker =
       (Get: {let get: string => Future.t(Result.t(Js.Json.t, string));}) => {
  let getFromMempool = (account: PublicKeyHash.t, network, operations) => {
    let%FtMap res = network->URL.Explorer.mempool(~account)->Get.get;
    let%Res res = res;
    let%ResMap mempool =
      res
      ->ResultEx.fromExn(
          Json.Decode.(array(Operation.Read.decodeFromMempool)),
        )
      ->ResultEx.mapError(Operation.Read.filterJsonExn);

    module Comparator = Operation.Read.Comparator;
    let operations =
      Set.fromArray(operations, ~id=(module Operation.Read.Comparator));
    let operations = mempool->Array.reduce(operations, Set.add)->Set.toArray;
    operations;
  };

  let getOperations =
      (
        network,
        account: PublicKeyHash.t,
        ~types: option(array(string))=?,
        ~destination: option(PublicKeyHash.t)=?,
        ~limit: option(int)=?,
        ~mempool: bool=false,
        (),
      ) => {
    let%FRes res =
      network
      ->URL.Explorer.operations(account, ~types?, ~destination?, ~limit?, ())
      ->Get.get;

    let%FRes operations =
      res
      ->ResultEx.fromExn(Json.Decode.(array(Operation.Read.decode)))
      ->ResultEx.mapError(Operation.Read.filterJsonExn)
      ->Future.value;

    mempool
      ? getFromMempool(account, network, operations) : operations->FutureEx.ok;
  };
};

module Explorer = ExplorerMaker(URL);
