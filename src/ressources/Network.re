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

type chain = string;

type apiVersion = {
  api: Version.t,
  indexer: string,
  node: string,
  chain,
  protocol: string,
};

let apiLowestBound = Version.mk(~fix=0, 2, 0);

let apiHighestBound = Version.mk(2, 0);

let checkInBound = version =>
  Version.checkInBound(version, apiLowestBound, apiHighestBound);

type monitorResult = {
  nodeLastBlock: int,
  nodeLastBlockTimestamp: string,
  indexerLastBlock: int,
  indexerLastBlockTimestamp: string,
};

type error =
  | APINotAvailable(string)
  | APIVersionFormat(string)
  | APIVersionRPCError(string)
  | APIMonitorRPCError(string)
  | NodeNotAvailable(string)
  | NodeChainRPCError(string)
  | ChainInconsistency(string, string)
  | UnknownChainId(string)
  | APINotSupported(Version.t);

let errorMsg =
  fun
  | APINotAvailable(_) => I18n.network#api_not_available
  | APIVersionRPCError(err) => I18n.network#api_version_rpc_error(err)
  | APIVersionFormat(v) => I18n.network#api_version_format_error(v)
  | APIMonitorRPCError(err) => I18n.network#api_monitor_rpc_error(err)
  | NodeNotAvailable(_) => I18n.network#node_not_available
  | NodeChainRPCError(err) => I18n.network#node_version_rpc_error(err)
  | ChainInconsistency(api, node) =>
    I18n.network#chain_inconsistency(api, node)
  | UnknownChainId(chain_id) => I18n.network#unknown_chain_id(chain_id)
  | APINotSupported(v) =>
    I18n.network#api_not_supported(Version.toString(v));

let mainnetChain = "NetXdQprcVkpaWU";
let granadanetChain = "NetXz969SFaFn8k";
let florencenetChain = "NetXxkAx4woPLyu";
let edo2netChain = "NetXSgo1ZT2DRUG";

let supportedChains = [
  mainnetChain,
  granadanetChain,
  florencenetChain,
  edo2netChain,
];

let fetchJson = (url, mkError) =>
  url
  ->Fetch.fetch
  ->FutureJs.fromPromise(err => mkError(Js.String.make(err)))
  ->Future.flatMapOk(response =>
      response
      ->Fetch.Response.json
      ->FutureJs.fromPromise(err => mkError(Js.String.make(err)))
    );

let monitor = url => {
  (url ++ "/monitor/blocks")
  ->fetchJson(e => APINotAvailable(e))
  ->Future.flatMapOk(json => {
      (
        try(
          Json.Decode.{
            nodeLastBlock: json |> field("node_last_block", int),
            nodeLastBlockTimestamp:
              json |> field("node_last_block_timestamp", string),
            indexerLastBlock: json |> field("indexer_last_block", int),
            indexerLastBlockTimestamp:
              json |> field("indexer_last_block_timestamp", string),
          }
          ->Ok
        ) {
        | Json.ParseError(error) => Error(APIMonitorRPCError(error))
        | Json.Decode.DecodeError(error) => Error(APIMonitorRPCError(error))
        | _ => Error(APIMonitorRPCError("Unknown error"))
        }
      )
      ->Future.value
    });
};

let getAPIVersion = url =>
  (url ++ "/version")
  ->fetchJson(e => APINotAvailable(e))
  ->Future.flatMapOk(json =>
      (
        try(
          Json.Decode.(field("api", string, json))
          ->Version.parse
          ->ResultEx.mapError((VersionFormat(e)) =>
              Error(APIVersionFormat(e))
            )
          ->Result.map(api =>
              Json.Decode.{
                api,
                indexer: json |> field("indexer", string),
                node: json |> field("node", string),
                chain: json |> field("chain", string),
                protocol: json |> field("protocol", string),
              }
            )
        ) {
        | Json.ParseError(error) => Error(APIVersionRPCError(error))
        | Json.Decode.DecodeError(error) => Error(APIVersionRPCError(error))
        | _ => Error(APIMonitorRPCError("Unknown error"))
        }
      )
      ->Future.value
    );

let getNodeChain = url => {
  (url ++ "/chains/main/chain_id")
  ->fetchJson(e => NodeNotAvailable(e))
  ->Future.flatMapOk(json => {
      switch (Js.Json.decodeString(json)) {
      | Some(v) => Future.value(Ok(v))
      | _ => Future.value(Error(APIVersionRPCError("not a Json string")))
      }
    });
};

let isMainnet = n => n == `Mainnet;
let isTestnet =
  fun
  | `Mainnet => false
  | `Testnet(_) => true;

let networkOfChain = c =>
  switch (supportedChains->List.getBy(chain => c == chain)) {
  | Some(c) => c == mainnetChain ? Ok(`Mainnet) : Ok(`Testnet(c))
  | None => Error(UnknownChainId(c))
  };

let checkConfiguration = (~network, api_url, node_url) =>
  Future.map2(
    getAPIVersion(api_url), getNodeChain(node_url), (api_res, node_res) =>
    switch (api_res, node_res) {
    | (Error(err), _)
    | (_, Error(err)) => Error(err)
    | (Ok(apiVersion), Ok(nodeChain)) =>
      String.equal(apiVersion.chain, nodeChain)
      && (
        isMainnet(network)
        && String.equal(nodeChain, mainnetChain)
        || isTestnet(network)
        && !String.equal(nodeChain, mainnetChain)
      )
        ? Ok(apiVersion)
        : Error(ChainInconsistency(apiVersion.chain, nodeChain))
    }
  );
