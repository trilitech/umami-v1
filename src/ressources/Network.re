type chain = string;

// Major, minor, fixes, specific patch/branch of the API
type versionNumber =
  | Version(int, int, option(int), option(string));

type apiVersion = {
  api: versionNumber,
  indexer: string,
  node: string,
  chain,
  protocol: string,
};

let mkVersion = (~fix=?, ~patch=?, major, minor) =>
  Version(major, minor, fix, patch);

let versionToString = (Version(major, minor, fix, patch)) =>
  Int.toString(major)
  ++ "."
  ++ Int.toString(minor)
  ++ fix
     ->Option.map(fix => "." ++ Int.toString(fix))
     ->Option.getWithDefault("")
  ++ patch->Option.map(patch => "~" ++ patch)->Option.getWithDefault("");

let apiLowestBound = mkVersion(~fix=0, 1, 0);

let apiHighestBound = mkVersion(1, Js.Int.max);

let leqFix =
  fun
  | (_, None) => true
  | (None, Some(_)) => false
  | (Some(fix1), Some(fix2)) => fix1 <= fix2;

let leqVersion =
    (Version(major1, minor1, fix1, _), Version(major2, minor2, fix2, _)) =>
  major1 <= major2 && minor1 <= minor2 && leqFix((fix1, fix2));

let checkInBound = version =>
  leqVersion(apiLowestBound, version) && leqVersion(version, apiHighestBound);

let mainnetChainId = "NetXdQprcVkpaWU";

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
  | APINotSupported(versionNumber);

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
  | APINotSupported(v) => I18n.network#api_not_supported(versionToString(v));

let mainnetChain = "NetXdQprcVkpaWU";
let florencenetChain = "NetXxkAx4woPLyu";
let edo2netChain = "NetXSgo1ZT2DRUG";

let parseVersion = version => {
  let parseInt = value =>
    value
    ->Int.fromString
    ->ResultEx.fromOption(Error(APIVersionFormat(version)));

  // parse a value of the form "<int>~patch", where ~patch is optional
  let parseFixAndPatch = value =>
    switch (value->Js.String2.splitAtMost(~limit=1, "~")) {
    | [|fix|] => fix->parseInt->Result.map(fix => (fix, None))
    | [|fix, patch|] => fix->parseInt->Result.map(fix => (fix, Some(patch)))
    | _ => Error(APIVersionFormat(version))
    };

  switch (version->Js.String2.split(".")) {
  | [|major, minor|] =>
    ResultEx.map2(major->parseInt, minor->parseInt, mkVersion)

  | [|major, minor, fixAndPatch|] =>
    fixAndPatch
    ->parseFixAndPatch
    ->Result.flatMap(((fix, patch)) =>
        ResultEx.map2(
          major->parseInt,
          minor->parseInt,
          mkVersion(~fix, ~patch?),
        )
      )

  | _ => Error(APIVersionFormat(version))
  };
};

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
          ->parseVersion
          // this two lines should disappear once mezos returns a valid version
          ->Result.getWithDefault(mkVersion(1, 0))
          ->Ok
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
  c == mainnetChain
    ? Ok(`Mainnet)
    : c == edo2netChain
        ? Ok(`Testnet(edo2netChain))
        : c == florencenetChain
            ? Ok(`Testnet(florencenetChain)) : Error(UnknownChainId(c));

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
