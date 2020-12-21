include ApiRequest;
module TokensAPI = API.Tokens(API.TezosClient);

type injection = {
  operation: Tokens.operation,
  password: string,
};

let useCheckTokenContract = (~network) => {
  let set = (~config, address) =>
    (network, config)->TokensAPI.checkTokenContract(address);
  ApiRequest.useSetter(~set, ~kind=Logs.Tokens, ~toast=false, ());
};

let useLoadOperationOffline =
    (
      ~network,
      ~requestState as (request, setRequest),
      ~operation: option(Tokens.operation),
    ) => {
  let get = (~config, (network, operation)) =>
    (network, config)->TokensAPI.callGetOperationOffline(operation);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Tokens, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect4(
    () => {
      let shouldReload = ApiRequest.conditionToLoad(request, isMounted);
      operation->Common.Lib.Option.iter(operation =>
        if (shouldReload) {
          getRequest((network, operation));
        }
      );
      None;
    },
    (isMounted, request, network, operation),
  );

  request;
};

let useLoadRegisteredTokens = (~network, ~requestState) => {
  let get = (~config as _c, network) =>
    TokensAPI.get(network)
    ->Future.mapOk(response => {
        response
        ->Belt.Array.map(((alias, symbol, address)) => {
            let token: Token.t = {alias, symbol, address};
            (address, token);
          })
        ->Belt.Map.String.fromArray
      });

  ApiRequest.useLoader1(~get, ~kind=Logs.Tokens, ~requestState, network);
};

let tokensStorageKey = "wallet-tokens";

let useLoadTokens = (~requestState) => {
  let get = (~config as _c, _) =>
    LocalStorage.getItem(tokensStorageKey)
    ->Js.Nullable.toOption
    ->Belt.Option.mapWithDefault([||], storageString =>
        storageString->Js.Json.parseExn->Token.Decode.array
      )
    ->Belt.Array.map(token => {(token.address, token)})
    ->Belt.Map.String.fromArray
    ->Belt.Result.Ok
    ->Future.value;

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState);
};

let useCreate = (~sideEffect=?, ()) => {
  let set = (~config as _c, token) => {
    let tokens =
      LocalStorage.getItem(tokensStorageKey)
      ->Js.Nullable.toOption
      ->Belt.Option.mapWithDefault([||], storageString =>
          storageString->Js.Json.parseExn->Token.Decode.array
        );

    LocalStorage.setItem(
      tokensStorageKey,
      tokens
      ->Belt.Array.concat([|token|])
      ->Token.Encode.array
      ->Js.Json.stringify,
    )
    ->Belt.Result.Ok
    ->Future.value;
  };

  ApiRequest.useSetter(
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};
