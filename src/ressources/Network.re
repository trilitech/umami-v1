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

type chain = string;

type apiVersion = {
  api: Version.t,
  indexer: string,
  node: string,
  chain,
  protocol: string,
};

let apiLowestBound = Version.mk(~fix=0, 2, 1);

let apiHighestBound = Version.mk(2, 1);

let checkInBound = version =>
  Version.checkInBound(version, apiLowestBound, apiHighestBound);

type monitorResult = {
  nodeLastBlock: int,
  nodeLastBlockTimestamp: string,
  indexerLastBlock: int,
  indexerLastBlockTimestamp: string,
};

type httpError = [ | `HttpError(string) | `AbortError];

type jsonError = [ | `JsonError(string)];

type apiError =
  | VersionFormat(string)
  | NotAvailable([ httpError | jsonError])
  | VersionRPCError(string)
  | MonitorRPCError(string)
  | NotSupported(Version.t);

type nodeError =
  | NotAvailable([ httpError | jsonError])
  | ChainRPCError(string)
  | VersionRPCError(string);

type Errors.t +=
  | UnknownChainId(string)
  | ChainInconsistency(string, string)
  | APIAndNodeError(apiError, nodeError)
  | API(apiError)
  | Node(nodeError);

let () =
  Errors.registerHandler(
    "Network",
    fun
    | API(NotAvailable(_)) => I18n.network#api_not_available->Some
    | API(VersionRPCError(err)) =>
      I18n.network#api_version_rpc_error(err)->Some
    | API(VersionFormat(v)) => I18n.network#api_version_format_error(v)->Some
    | API(MonitorRPCError(err)) =>
      I18n.network#api_monitor_rpc_error(err)->Some
    | Node(NotAvailable(_)) => I18n.network#node_not_available->Some
    | Node(ChainRPCError(err)) =>
      I18n.network#node_version_rpc_error(err)->Some
    | Node(VersionRPCError(err)) =>
      I18n.network#node_version_rpc_error(err)->Some
    | ChainInconsistency(api, node) =>
      I18n.network#chain_inconsistency(api, node)->Some
    | UnknownChainId(chain_id) =>
      I18n.network#unknown_chain_id(chain_id)->Some
    | API(NotSupported(v)) =>
      I18n.network#api_not_supported(Version.toString(v))->Some
    | APIAndNodeError(_, _) => I18n.network#api_and_node_not_available->Some
    | _ => None,
  );

let mainnetChain = "NetXdQprcVkpaWU";
let hangzhounetChain = "NetXuXoGoLxNK6o";
let granadanetChain = "NetXz969SFaFn8k";
let florencenetChain = "NetXxkAx4woPLyu";
let edo2netChain = "NetXSgo1ZT2DRUG";

type network = {
  name: string,
  chain: string,
  explorer: string,
  endpoint: string,
};

let mainnet = {
  name: I18n.t#mainnet,
  chain: mainnetChain,
  explorer: "https://api.umamiwallet.com/mainnet",
  endpoint: "https://mainnet.smartpy.io/",
};

let granadanet = {
  name: I18n.t#granadanet,
  chain: granadanetChain,
  explorer: "https://api.umamiwallet.com/granadanet",
  endpoint: "https://granadanet.smartpy.io/",
};

let supportedChains = [
  mainnetChain,
  hangzhounetChain,
  granadanetChain,
  florencenetChain,
  edo2netChain,
];

let mainnetName = "mainnet";
let hangzhounetName = "hangzhounet";
let granadanetName = "granadanet";
let florencenetName = "florencenet";
let edo2netName = "edo2net";

let getName = network =>
  switch (network) {
  | network when network == mainnetChain => mainnetName
  | network when network == granadanetChain => granadanetName
  | network when network == hangzhounetChain => hangzhounetName
  | network when network == florencenetChain => florencenetName
  | network when network == edo2netChain => edo2netName
  | _ => ""
  };

type requestInit = {signal: Fetch.signal};

[@bs.val]
external fetch: (string, requestInit) => Js.Promise.t(Fetch.response) =
  "fetch";
let fetch = (url, ~timeout=?, ()) => {
  open UmamiCommon;
  let ctrl = Fetch.AbortController.make();
  let signal = Fetch.AbortController.signal(ctrl);
  let res = fetch(url, {signal: signal});
  timeout->Lib.Option.iter(ms => {
    let _: Js_global.timeoutId =
      Js.Global.setTimeout(() => Fetch.AbortController.abort(ctrl), ms);
    ();
  });
  res->FutureJs.fromPromise(err => {
    let {name} = err->RawJsError.fromPromiseError;
    switch (name) {
    | "AbortError" => `AbortError
    | e => `HttpError(e)
    };
  });
};

let fetchJson = (url, ~timeout=?, mkError) =>
  url
  ->fetch(~timeout?, ())
  ->Future.mapError(e => mkError(e))
  ->Future.flatMapOk(response =>
      response
      ->Fetch.Response.json
      ->FutureJs.fromPromise(err => mkError(Js.String.make(err)->`JsonError))
    );

let mapAPIError: _ => Errors.t =
  fun
  | Json.ParseError(error) => API(MonitorRPCError(error))
  | Json.Decode.DecodeError(error) => API(MonitorRPCError(error))
  | _ => API(MonitorRPCError("Unknown error"));

let monitor = url => {
  let%FlatRes json =
    (url ++ "/monitor/blocks")->fetchJson(e => API(NotAvailable(e)));

  ResultEx.fromExn((), () =>
    Json.Decode.{
      nodeLastBlock: json |> field("node_last_block", int),
      nodeLastBlockTimestamp:
        json |> field("node_last_block_timestamp", string),
      indexerLastBlock: json |> field("indexer_last_block", int),
      indexerLastBlockTimestamp:
        json |> field("indexer_last_block_timestamp", string),
    }
  )
  ->ResultEx.mapError(mapAPIError);
};

let getAPIVersion = (~timeout=?, url) => {
  let%FlatRes json =
    (url ++ "/version")->fetchJson(~timeout?, e => API(NotAvailable(e)));

  let%Res api =
    ResultEx.fromExn((), () => Json.Decode.(field("api", string, json)))
    ->ResultEx.mapError(mapAPIError);

  let%Res api =
    Version.parse(api)
    ->ResultEx.mapError((VersionFormat(e)) => API(VersionFormat(e)));

  ResultEx.fromExn((), () =>
    Json.Decode.{
      api,
      indexer: json |> field("indexer", string),
      node: json |> field("node", string),
      chain: json |> field("chain", string),
      protocol: json |> field("protocol", string),
    }
  )
  ->ResultEx.mapError(mapAPIError);
};

let getChainName = chain => {
  switch (chain) {
  | chain when chain == mainnetChain => "Mainnet"
  | chain when chain == hangzhounetChain => "HangzhouNet"
  | chain when chain == granadanetChain => "GranadaNet"
  | chain when chain == florencenetChain => "Florencenet"
  | chain when chain == edo2netChain => "Edo2Net"
  | _ => ""
  };
};

let getNodeChain = (~timeout=?, url) => {
  let%FlatRes json =
    (url ++ "/chains/main/chain_id")
    ->fetchJson(~timeout?, e => Node(NotAvailable(e)));
  switch (Js.Json.decodeString(json)) {
  | Some(v) => Ok(v)
  | _ => VersionRPCError("not a Json string")->Node->Error
  };
};

let isMainnet = n => n == `Mainnet;

let networkOfChain = c =>
  switch (supportedChains->List.getBy(chain => c == chain)) {
  | Some(c) => c == mainnetChain ? Ok(`Mainnet) : Ok(`Testnet(c))
  | None => Error(UnknownChainId(c))
  };

let checkConfiguration =
    (api_url, node_url): Future.t(Result.t((apiVersion, string), Errors.t)) =>
  Future.map2(
    getAPIVersion(~timeout=5000, api_url),
    getNodeChain(~timeout=5000, node_url),
    (api_res, node_res) =>
    switch (api_res, node_res) {
    | (Error(API(api_err)), Error(Node(node_err))) =>
      Error(APIAndNodeError(api_err, node_err))
    | (Error(err), _)
    | (_, Error(err)) => Error(err)
    | (Ok(apiVersion), Ok(nodeChain)) =>
      String.equal(apiVersion.chain, nodeChain)
        ? Ok((apiVersion, nodeChain))
        : Error(ChainInconsistency(apiVersion.chain, nodeChain))
    }
  );
