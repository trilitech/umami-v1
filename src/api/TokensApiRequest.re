include ApiRequest;
module TokensAPI = API.Tokens(API.TezosClient);

type injection = {
  operation: Token.operation,
  password: string,
};

let useCheckTokenContract = () => {
  let set = (~settings, address) =>
    settings->TokensAPI.checkTokenContract(address);
  ApiRequest.useSetter(~set, ~kind=Logs.Tokens, ~toast=false, ());
};

let useLoadOperationOffline =
    (
      ~requestState as (request, setRequest),
      ~operation: option(Token.operation),
    ) => {
  let get = (~settings, operation) =>
    settings->TokensAPI.callGetOperationOffline(operation);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Tokens, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect3(
    () => {
      let shouldReload = ApiRequest.conditionToLoad(request, isMounted);
      operation->Common.Lib.Option.iter(operation =>
        if (shouldReload) {
          getRequest(operation);
        }
      );
      None;
    },
    (isMounted, request, operation),
  );

  request;
};

let useLoadRegisteredTokens = (~requestState) => {
  let get = (~settings, ()) =>
    TokensAPI.get(settings)
    ->Future.mapOk(response => {
        response
        ->Belt.Array.map(((alias, symbol, address)) => {
            let token: Token.t = {alias, symbol, address};
            (address, token);
          })
        ->Belt.Map.String.fromArray
      });

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState);
};

let tokensStorageKey = "wallet-tokens";

let useLoadTokens = requestState => {
  let get = (~settings as _, ()) =>
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
  let set = (~settings as _, token) => {
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
