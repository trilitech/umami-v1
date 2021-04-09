type chain = string;

type apiVersion = {
  api: string,
  indexer: string,
  node: string,
  chain,
  protocol: string,
};

let mainnetChainId = "NetXdQprcVkpaWU";

type monitorResult = {
  nodeLastBlock: int,
  nodeLastBlockTimestamp: string,
  indexerLastBlock: int,
  indexerLastBlockTimestamp: string,
};

type error =
  | APINotAvailable(string)
  | APIVersionRPCError(string)
  | NodeNotAvailable(string)
  | NodeChainRPCError(string)
  | ChainInconsistency(string, string)
  | UnknownChainId(string);

let errorMsg =
  fun
  | APINotAvailable(_) => I18n.network#api_not_available
  | APIVersionRPCError(err) => I18n.network#api_version_rpc_error(err)
  | NodeNotAvailable(_) => I18n.network#node_not_available
  | NodeChainRPCError(err) => I18n.network#node_version_rpc_error(err)
  | ChainInconsistency(api, node) =>
    I18n.network#chain_inconsistency(api, node)
  | UnknownChainId(chain_id) => I18n.network#unknown_chain_id(chain_id);

let mainnetChain = "NetXdQprcVkpaWU";
let florencenetChain = "NetXxkAx4woPLyu";
let edo2netChain = "NetXSgo1ZT2DRUG";

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
        | Json.ParseError(error) => Error(APIVersionRPCError(error))
        | Json.Decode.DecodeError(error) => Error(APIVersionRPCError(error))
        | _ => Error(APIVersionRPCError("Unknown error"))
        }
      )
      ->Future.value
    });
};

let getAPIChain = url =>
  (url ++ "/version")
  ->fetchJson(e => APINotAvailable(e))
  ->Future.flatMapOk(json => {
      let obj =
        json
        ->Js.Json.decodeObject
        ->Option.flatMap(d => d->Js.Dict.get("chain"))
        ->Option.flatMap(Js.Json.decodeString);

      switch (obj) {
      | Some(v) => Future.value(Ok(v))
      | _ =>
        Future.value(Error(APIVersionRPCError("not a valid Json object")))
      };
    });

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
  c == mainnetChain
    ? Ok(`Mainnet)
    : c == edo2netChain
        ? Ok(`Testnet(edo2netChain))
        : c == florencenetChain
            ? Ok(`Testnet(florencenetChain)) : Error(UnknownChainId(c));

let checkConfiguration = (~network, api_url, node_url) =>
  Future.map2(
    getAPIChain(api_url), getNodeChain(node_url), (api_res, node_res) =>
    switch (api_res, node_res) {
    | (Error(err), _)
    | (_, Error(err)) => Error(err)
    | (Ok(api_chain), Ok(node_chain)) =>
      String.equal(api_chain, node_chain)
      && (
        isMainnet(network)
        && String.equal(node_chain, mainnetChain)
        || isTestnet(network)
        && !String.equal(node_chain, mainnetChain)
      )
        ? networkOfChain(api_chain)
        : Error(ChainInconsistency(api_chain, node_chain))
    }
  );
