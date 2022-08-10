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
    | API(NotAvailable(_)) => I18n.Network.api_not_available->Some
    | API(VersionRPCError(err)) =>
      I18n.Network.api_version_rpc_error(err)->Some
    | API(VersionFormat(v)) => I18n.Network.api_version_format_error(v)->Some
    | API(MonitorRPCError(err)) =>
      I18n.Network.api_monitor_rpc_error(err)->Some
    | Node(NotAvailable(_)) => I18n.Network.node_not_available->Some
    | Node(ChainRPCError(err)) =>
      I18n.Network.node_version_rpc_error(err)->Some
    | Node(VersionRPCError(err)) =>
      I18n.Network.node_version_rpc_error(err)->Some
    | ChainInconsistency(api, node) =>
      I18n.Network.chain_inconsistency(api, node)->Some
    | UnknownChainId(chain_id) =>
      I18n.Network.unknown_chain_id(chain_id)->Some
    | API(NotSupported(v)) =>
      I18n.Network.api_not_supported(Version.toString(v))->Some
    | APIAndNodeError(_, _) => I18n.Network.api_and_node_not_available->Some
    | EndpointError => I18n.Errors.no_valid_endpoint->Some
    | _ => None,
  );

type nativeChains = [ | `Mainnet | `Ghostnet | `Jakartanet];

type supportedChains = [
  nativeChains
  | `Hangzhounet
  | `Florencenet
  | `Edo2net
  | `Granadanet
];

let unsafeChainId: string => chainId = c => c;

let getChainId =
  fun
  | `Jakartanet => "NetXLH1uAxK7CCh"
  | `Ghostnet => "NetXnHfVqm9iesp"
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
  | "NetXnHfVqm9iesp" => `Ghostnet
  | "NetXLH1uAxK7CCh" => `Jakartanet
  | s => `Custom(s);

let mkChainAssoc = c => (c, getChainId(c));

let nativeChains = [
  mkChainAssoc(`Mainnet),
  mkChainAssoc(`Ghostnet),
  mkChainAssoc(`Jakartanet),
];

let supportedChains = [
  mkChainAssoc(`Florencenet),
  mkChainAssoc(`Hangzhounet),
  mkChainAssoc(`Edo2net),
  mkChainAssoc(`Granadanet),
  ...nativeChains,
];

let getDisplayedName =
  fun
  | `Granadanet => "Granadanet"
  | `Mainnet => "Mainnet"
  | `Florencenet => "Florencenet"
  | `Edo2net => "Edo2net"
  | `Hangzhounet => "Hangzhounet"
  | `Ghostnet => "Ghostnet"
  | `Jakartanet => "Jakartanet"
  | `Custom(s) => s;

let externalExplorer =
  fun
  | `Mainnet => "https://tzkt.io/"->Ok
  | `Edo2net => "https://edo2net.tzkt.io/"->Ok
  | `Florencenet => "https://florencenet.tzkt.io/"->Ok
  | `Granadanet => "https://granadanet.tzkt.io/"->Ok
  | `Hangzhounet => "https://hangzhou2net.tzkt.io/"->Ok
  | `Ghostnet => "https://ghostnet.tzkt.io/"->Ok
  | `Jakartanet => "https://jakartanet.tzkt.io/"->Ok
  | `Custom(_) as net => Error(UnknownChainId(getChainId(net)));

type chain('chainId) = [ supportedChains | `Custom('chainId)];
type configurableChains('chainId) = [ nativeChains | `Custom('chainId)];

type network = {
  name: string,
  chain: chain(chainId),
  explorer: string,
  endpoint: string,
};

type t = network;

let chainNetwork: chain(_) => option(string) =
  fun
  | `Mainnet => Some("mainnet")
  | `Granadanet => Some("granadanet")
  | `Florencenet => Some("florencenet")
  | `Edo2net => Some("edo2net")
  | `Hangzhounet => Some("hangzhou2net")
  | `Ghostnet => Some("ghostnet")
  | `Jakartanet => Some("jakartanet")
  | `Custom(_) => None;

let networkChain: string => option(chain(_)) =
  fun
  | "mainnet" => Some(`Mainnet)
  | "granadanet" => Some(`Granadanet)
  | "florencenet" => Some(`Florencenet)
  | "edo2net" => Some(`Edo2net)
  | "hangzhou2net" => Some(`Hangzhounet)
  | "ithacanet" /* Kill? */
  | "ghostnet" => Some(`Ghostnet)
  | "jakartanet" => Some(`Jakartanet)
  | _ => None;

module Encode = {
  let chainToString = getDisplayedName;

  let chainKind =
    fun
    | `Mainnet
    | `Granadanet
    | `Florencenet
    | `Edo2net
    | `Ghostnet
    | `Jakartanet
    | `Hangzhounet => "default"
    | `Custom(_) => "custom";

  let chainIdEncoder: Json.Encode.encoder(chainId) = Json.Encode.string;

  let chainEncoder = n =>
    Json.Encode.(
      object_([
        ("kind", string(chainKind(n))),
        ("name", string(chainToString(n))),
      ])
    );

  let chainEncoderString = chainEncoder;

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
    | "Ithacanet" /* Kill? */
    | "Ghostnet" => `Ghostnet
    | "Jakartanet" => `Jakartanet
    | n => raise(Json.Decode.DecodeError("Unknown network " ++ n));

  let chainFromString =
    fun
    | "Florencenet" => `Florencenet
    | "Edo2net" => `Edo2net
    | "Hangzhounet" => `Hangzhounet
    | n => nativeChainFromString(n);

  let chainIdDecoder: Json.Decode.decoder(chainId) = Json.Decode.string;

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
           | v => raise(Json.Decode.DecodeError("Unknown kind" ++ v)),
         )
    );
  };

  let chainDecoderString = chainDecoder;

  let decoder = json =>
    Json.Decode.{
      name: json |> field("name", string),
      chain: json |> field("chain", chainDecoder(chainFromString)),
      explorer: json |> field("explorer", string),
      endpoint: json |> field("endpoint", string),
    };
};

let mk = (~explorer, ~endpoint, chain) => {
  name: getDisplayedName(chain),
  chain,
  explorer,
  endpoint,
};

let mainnet =
  mk(
    ~explorer="https://mainnet.umamiwallet.com",
    ~endpoint="https://mainnet.smartpy.io/",
    `Mainnet,
  );

let ghostnet =
  mk(
    ~explorer="https://ghostnet.umamiwallet.com",
    ~endpoint="https://ghostnet.ecadinfra.com/",
    `Ghostnet,
  );

let jakartanet =
  mk(
    ~explorer="https://jakartanet.umamiwallet.com/",
    ~endpoint="https://jakartanet.ecadinfra.com/",
    `Jakartanet,
  );

let getNetwork =
  fun
  | `Mainnet => mainnet
  | `Ghostnet => ghostnet
  | `Jakartanet => jakartanet;

let getNetworks = (c: nativeChains) => {
  let n = getNetwork(c);
  let withEP = url => {...n, endpoint: url};
  switch (c) {
  | `Mainnet => [
      withEP("https://mainnet.smartpy.io/"),
      withEP("https://api.tez.ie/rpc/mainnet/"),
      withEP("https://teznode.letzbake.com"),
      withEP("https://mainnet.tezrpc.me/"),
      withEP("https://rpc.tzbeta.net"),
    ]
  | `Ghostnet => [
      withEP("https://ghostnet.ecadinfra.com/"),
      withEP("https://ghostnet.smartpy.io/"),
    ]
  | `Jakartanet => [
      withEP("https://jakartanet.ecadinfra.com/"),
      withEP("https://rpczero.tzbeta.net"),
      withEP("https://jakartanet.tezos.marigold.dev/"),
    ]
  };
};

let testNetwork = n =>
  // Avoids dependency to TaquitoAPI
  ReTaquito.RPCClient.create(n.endpoint)
  ->ReTaquito.RPCClient.getBlockHeader()
  ->ReTaquitoError.fromPromiseParsed
  ->Promise.timeoutAfter(2000);

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
  (url ++ "/monitor/blocks")
  ->fetchJson(e => API(NotAvailable(e)))
  ->Promise.flatMapOk(json =>
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
      ->Result.mapError(mapAPIError)
      ->Promise.value
    );
};

let getAPIVersion = (~timeout=?, url) => {
  (url ++ "/version")
  ->fetchJson(~timeout?, e => API(NotAvailable(e)))
  ->Promise.flatMapOk(json =>
      Result.fromExn((), () => Json.Decode.(field("api", string, json)))
      ->Result.mapError(mapAPIError)
      ->Promise.value
      ->Promise.flatMapOk(api =>
          Version.parse(api)
          ->Result.mapError(
              fun
              | Version.VersionFormat(e) => API(VersionFormat(e))
              | _ => API(VersionRPCError("Unknown error")),
            )
          ->Promise.value
        )
      ->Promise.flatMapOk(api =>
          Result.fromExn((), () =>
            Json.Decode.{
              api,
              indexer: json |> field("indexer", string),
              node: json |> field("node", string),
              chain: json |> field("chain", string),
              protocol: json |> field("protocol", string),
            }
          )
          ->Result.mapError(mapAPIError)
          ->Promise.value
        )
    );
};

let getNodeChain = (~timeout=?, url) => {
  (url ++ "/chains/main/chain_id")
  ->fetchJson(~timeout?, e => Node(NotAvailable(e)))
  ->Promise.flatMapOk(json =>
      switch (Js.Json.decodeString(json)) {
      | Some(v) =>
        let chain =
          supportedChains
          ->List.getBy(((_, id)) => id == v)
          ->Option.map(fst)
          ->Option.getWithDefault(`Custom(v));

        Promise.ok(chain);
      | _ => VersionRPCError("not a Json string")->Node->Promise.err
      }
    );
};

let checkConfiguration =
    (api_url, node_url): Promise.t((apiVersion, chain(chainId))) =>
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
