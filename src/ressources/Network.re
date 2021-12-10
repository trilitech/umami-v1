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

type chainId = string;

type apiVersion = {
  api: Version.t,
  indexer: string,
  node: string,
  chain: chainId,
  protocol: string,
};

let apiLowestBound = Version.mk(~fix=0, 2, 2);

let apiHighestBound = Version.mk(2, 2);

let checkInBound = version =>
  Version.checkInBound(version, apiLowestBound, apiHighestBound);

type monitorResult = {
  nodeLastBlock: int,
  nodeLastBlockTimestamp: string,
  indexerLastBlock: int,
  indexerLastBlockTimestamp: string,
};

type status =
  | Online
  | Pending
  | Offline;

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
  | EndpointError
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
    | EndpointError => I18n.errors#no_valid_endpoint->Some
    | _ => None,
  );

type nativeChains = [ | `Hangzhounet | `Mainnet];

type supportedChains = [ nativeChains | `Florencenet | `Edo2net | `Granadanet];

let getChainId =
  fun
  | `Granadanet => "NetXz969SFaFn8k"
  | `Mainnet => "NetXdQprcVkpaWU"
  | `Florencenet => "NetXxkAx4woPLyu"
  | `Edo2net => "NetXSgo1ZT2DRUG"
  | `Hangzhounet => "NetXZSsxBpMQeAT"
  | `Custom(s) => s;

let fromChainId =
  fun
  | "NetXz969SFaFn8k" => `Granadanet
  | "NetXdQprcVkpaWU" => `Mainnet
  | "NetXxkAx4woPLyu" => `Florencenet
  | "NetXSgo1ZT2DRUG" => `Edo2net
  | "NetXZSsxBpMQeAT" => `Hangzhounet
  | s => `Custom(s);

let nativeChains = [
  (`Mainnet, getChainId(`Mainnet)),
  (`Hangzhounet, getChainId(`Hangzhounet)),
];

let supportedChains = [
  (`Florencenet, getChainId(`Florencenet)),
  (`Edo2net, getChainId(`Edo2net)),
  (`Granadanet, getChainId(`Granadanet)),
  ...nativeChains,
];

let getDisplayedName =
  fun
  | `Granadanet => "Granadanet"
  | `Mainnet => "Mainnet"
  | `Florencenet => "Florencenet"
  | `Edo2net => "Edo2net"
  | `Hangzhounet => "Hangzhounet"
  | `Custom(s) => s;

let externalExplorer =
  fun
  | `Mainnet => "https://tzkt.io/"->Ok
  | `Edo2net => "https://edo2net.tzkt.io/"->Ok
  | `Florencenet => "https://florencenet.tzkt.io/"->Ok
  | `Granadanet => "https://granadanet.tzkt.io/"->Ok
  | `Hangzhounet => "https://hangzhounet.tzkt.io/"->Ok
  | `Custom(_) as net => Error(UnknownChainId(getChainId(net)));

type chain = [ supportedChains | `Custom(chainId)];
type configurableChains = [ nativeChains | `Custom(chainId)];

type network = {
  name: string,
  chain,
  explorer: string,
  endpoint: string,
};

let mk = (~name, ~explorer, ~endpoint, chain) => {
  name,
  chain,
  explorer,
  endpoint,
};

let chainNetwork: chain => option(string) =
  fun
  | `Mainnet => Some("mainnet")
  | `Granadanet => Some("granadanet")
  | `Florencenet => Some("florencenet")
  | `Edo2net => Some("edo2net")
  | `Hangzhounet => Some("hangzhou2net")
  | `Custom(_) => None;

let networkChain: string => option(chain) =
  fun
  | "mainnet" => Some(`Mainnet)
  | "granadanet" => Some(`Granadanet)
  | "florencenet" => Some(`Florencenet)
  | "edo2net" => Some(`Edo2net)
  | "hangzhou2net" => Some(`Hangzhounet)
  | _ => None;

module Encode = {
  let chainToString =
    fun
    | `Mainnet => "Mainnet"
    | `Granadanet => "Granadanet"
    | `Florencenet => "Florencenet"
    | `Edo2net => "Edo2net"
    | `Hangzhounet => "Hangzhounet"
    | `Custom(n) => n;

  let chainKind =
    fun
    | `Mainnet
    | `Granadanet
    | `Florencenet
    | `Edo2net
    | `Hangzhounet => "default"
    | `Custom(_) => "custom";

  let chainEncoder = n =>
    Json.Encode.(
      object_([
        ("kind", string(chainKind(n))),
        ("name", string(chainToString(n))),
      ])
    );

  let encoder = c =>
    Json.Encode.(
      object_([
        ("name", string(c.name)),
        ("chain", chainEncoder(c.chain)),
        ("explorer", string(c.explorer)),
        ("endpoint", string(c.endpoint)),
      ])
    );
};

module Decode = {
  let nativeChainFromString =
    fun
    | "Mainnet" => `Mainnet
    | "Hangzhounet" => `Hangzhounet
    | n =>
      JsonEx.(raise(InternalError(DecodeError("Unknown network " ++ n))));

  let chainFromString =
    fun
    | "Florencenet" => `Florencenet
    | "Edo2net" => `Edo2net
    | n => nativeChainFromString(n);

  let chainDecoder = (chainFromString, json) => {
    open Json.Decode;
    let defaultNetworkDecoder = json => json->string->chainFromString;
    let customNetworkDecoder = json => json->string->`Custom;
    json
    |> (
      field("kind", string)
      |> andThen(
           fun
           | "default" => field("name", defaultNetworkDecoder)
           | "custom" => field("name", customNetworkDecoder)
           | v =>
             JsonEx.(
               raise(InternalError(DecodeError("Unknown kind" ++ v)))
             ),
         )
    );
  };

  let decoder = json =>
    Json.Decode.{
      name: json |> field("name", string),
      chain: json |> field("chain", chainDecoder(chainFromString)),
      explorer: json |> field("explorer", string),
      endpoint: json |> field("endpoint", string),
    };
};

let mainnet =
  mk(
    ~name=getDisplayedName(`Mainnet),
    ~explorer="https://api.umamiwallet.com/mainnet",
    ~endpoint="https://mainnet.smartpy.io/",
    `Mainnet,
  );

let hangzhounet =
  mk(
    ~name=getDisplayedName(`Hangzhounet),
    ~explorer="https://api.umamiwallet.com/hangzhounet",
    ~endpoint="https://hangzhounet.smartpy.io/",
    `Hangzhounet,
  );

let withEP = (n, url) => {...n, endpoint: url};

let mainnetNetworks = [
  mainnet->withEP("https://mainnet.smartpy.io/"),
  mainnet->withEP("https://api.tez.ie/rpc/mainnet/"),
  mainnet->withEP("https://teznode.letzbake.com"),
  mainnet->withEP("https://mainnet.tezrpc.me/"),
  mainnet->withEP("https://rpc.tzbeta.net/"),
];

let hangzhounetNetworks = [
  hangzhounet->withEP("https://hangzhounet.smartpy.io/"),
  hangzhounet->withEP("https://api.tez.ie/rpc/hangzhounet"),
];

let getNetworks = (c: nativeChains) =>
  switch (c) {
  | `Hangzhounet => hangzhounetNetworks
  | `Mainnet => mainnetNetworks
  };

let testNetwork = n =>
  TaquitoAPI.Rpc.getBlockHeader(n.endpoint)->Promise.timeoutAfter(2000);

let testNetworks = eps => {
  let eps = eps->List.shuffle;
  let rec loop = l => {
    switch (l) {
    | [] => Promise.err(EndpointError)
    | [h, ...tl] =>
      h
      ->testNetwork
      ->Promise.flatMap(
          fun
          | Ok(_) => h->Promise.ok
          | Error(_) => loop(tl),
        )
    };
  };

  loop(eps);
};

let findValidEndpoint = chain => chain->getNetworks->testNetworks;

type requestInit = {signal: Fetch.signal};

[@bs.val]
external fetch: (string, requestInit) => Js.Promise.t(Fetch.response) =
  "fetch";
let fetch = (url, ~timeout=?, ()) => {
  let ctrl = Fetch.AbortController.make();
  let signal = Fetch.AbortController.signal(ctrl);
  let res = fetch(url, {signal: signal});
  timeout->Option.iter(ms => {
    let _: Js_global.timeoutId =
      Js.Global.setTimeout(() => Fetch.AbortController.abort(ctrl), ms);
    ();
  });
  res->Promise.fromJs(err => {
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
  ->Promise.mapError(e => mkError(e))
  ->Promise.flatMapOk(response =>
      response
      ->Fetch.Response.json
      ->Promise.fromJs(err => mkError(Js.String.make(err)->`JsonError))
    );

let mapAPIError: _ => Errors.t =
  fun
  | Json.ParseError(error) => API(MonitorRPCError(error))
  | Json.Decode.DecodeError(error) => API(MonitorRPCError(error))
  | _ => API(MonitorRPCError("Unknown error"));

let monitor = url => {
  let%AwaitRes json =
    (url ++ "/monitor/blocks")->fetchJson(e => API(NotAvailable(e)));

  Result.fromExn((), () =>
    Json.Decode.{
      nodeLastBlock: json |> field("node_last_block", int),
      nodeLastBlockTimestamp:
        json |> field("node_last_block_timestamp", string),
      indexerLastBlock: json |> field("indexer_last_block", int),
      indexerLastBlockTimestamp:
        json |> field("indexer_last_block_timestamp", string),
    }
  )
  ->Result.mapError(mapAPIError);
};

let getAPIVersion = (~timeout=?, url) => {
  let%AwaitRes json =
    (url ++ "/version")->fetchJson(~timeout?, e => API(NotAvailable(e)));

  let%Res api =
    Result.fromExn((), () => Json.Decode.(field("api", string, json)))
    ->Result.mapError(mapAPIError);

  let%Res api =
    Version.parse(api)
    ->Result.mapError(
        fun
        | Version.VersionFormat(e) => API(VersionFormat(e))
        | _ => API(VersionRPCError("Unknown error")),
      );

  Result.fromExn((), () =>
    Json.Decode.{
      api,
      indexer: json |> field("indexer", string),
      node: json |> field("node", string),
      chain: json |> field("chain", string),
      protocol: json |> field("protocol", string),
    }
  )
  ->Result.mapError(mapAPIError);
};

let getNodeChain = (~timeout=?, url) => {
  let%AwaitRes json =
    (url ++ "/chains/main/chain_id")
    ->fetchJson(~timeout?, e => Node(NotAvailable(e)));
  switch (Js.Json.decodeString(json)) {
  | Some(v) =>
    let chain =
      supportedChains
      ->List.getBy(((_, id)) => id == v)
      ->Option.map(fst)
      ->Option.getWithDefault(`Custom(v));

    Ok(chain);
  | _ => VersionRPCError("not a Json string")->Node->Error
  };
};

let isMainnet = n => n == `Mainnet;

let checkConfiguration = (api_url, node_url): Promise.t((apiVersion, chain)) =>
  Promise.map2(
    getAPIVersion(~timeout=5000, api_url),
    getNodeChain(~timeout=5000, node_url),
    (api_res, node_res) =>
    switch (api_res, node_res) {
    | (Error(API(api_err)), Error(Node(node_err))) =>
      Error(APIAndNodeError(api_err, node_err))
    | (Error(err), _)
    | (_, Error(err)) => Error(err)
    | (Ok(apiVersion), Ok(nodeChain)) =>
      String.equal(apiVersion.chain, nodeChain->getChainId)
        ? Ok((apiVersion, nodeChain))
        : Error(ChainInconsistency(apiVersion.chain, nodeChain->getChainId))
    }
  );
