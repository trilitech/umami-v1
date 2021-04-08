type chain = string;

type apiVersion = {
  api: string,
  indexer: string,
  node: string,
  chain,
  protocol: string,
};

let mainnetChainId = "NetXdQprcVkpaWU";

type error =
  | APINotAvailable(string)
  | APIVersionRPCError(string)
  | NodeNotAvailable(string)
  | NodeChainRPCError(string)
  | ChainInconsistency(string, string);

let errorMsg =
  fun
  | APINotAvailable(_) => I18n.network#api_not_available
  | APIVersionRPCError(err) => I18n.network#api_version_rpc_error(err)
  | NodeNotAvailable(_) => I18n.network#node_not_available
  | NodeChainRPCError(err) => I18n.network#node_version_rpc_error(err)
  | ChainInconsistency(api, node) =>
    I18n.network#chain_inconsistency(api, node);

let getAPIChain = url =>
  (url ++ "/version")
  ->Fetch.fetch
  ->FutureJs.fromPromise(err => APINotAvailable(Js.String.make(err)))
  ->Future.flatMapOk(response =>
      response
      ->Fetch.Response.json
      ->FutureJs.fromPromise(err => APINotAvailable(Js.String.make(err)))
    )
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
  ->Fetch.fetch
  ->FutureJs.fromPromise(err => NodeNotAvailable(Js.String.make(err)))
  ->Future.flatMapOk(response =>
      response
      ->Fetch.Response.json
      ->FutureJs.fromPromise(err => NodeNotAvailable(Js.String.make(err)))
    )
  ->Future.flatMapOk(json => {
      switch (Js.Json.decodeString(json)) {
      | Some(v) => Future.value(Ok(v))
      | _ => Future.value(Error(APIVersionRPCError("not a Json string")))
      }
    });
};

let checkConfiguration = (~network, api_url, node_url) =>
  Future.map2(
    getAPIChain(api_url), getNodeChain(node_url), (api_res, node_res) =>
    switch (api_res, node_res) {
    | (Error(err), _)
    | (_, Error(err)) => Error(err)
    | (Ok(api_chain), Ok(node_chain)) =>
      String.equal(api_chain, node_chain)
      && (
        network == `Mainnet
        && String.equal(node_chain, mainnetChainId)
        || network == `Testnet
        && !String.equal(node_chain, mainnetChainId)
      )
        ? Ok() : Error(ChainInconsistency(api_chain, node_chain))
    }
  );
