include ApiRequest;
module TokensAPI = API.Tokens(API.TezosClient);

type injection = {
  operation: Tokens.operation,
  password: string,
};

let useCreate = network => {
  let set = (~config, {operation, password}) =>
    (network, config)->TokensAPI.inject(operation, ~password);

  ApiRequest.useSetter(~set, ~kind=Logs.Tokens, ());
};

let useSimulate = network => {
  let set = (~config, operation) =>
    (network, config)->TokensAPI.simulate(operation);
  ApiRequest.useSetter(~set, ~kind=Logs.Tokens, ());
};

let useCheckTokenContract = TokensAPI.checkTokenContract;

let useGetOperationOffline =
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
      let (shouldReload, loading) =
        ApiRequest.conditionToLoad(request, isMounted);
      operation->Common.Lib.Option.iter(operation =>
        if (shouldReload) {
          getRequest(~loading, (network, operation));
        }
      );
      None;
    },
    (isMounted, request, network, operation),
  );

  request;
};

let useLoadTokens = (~network, ~requestState) => {
  let get = (~config as _c, network) =>
    TokensAPI.get(network)
    ->Future.mapOk(response => {
        response
        ->Belt.Array.map(((alias, currency, address)) => {
            let token: Token.t = {alias, currency, address};
            (address, token);
          })
        ->Belt.Map.String.fromArray
      });

  ApiRequest.useLoader1(~get, ~kind=Logs.Tokens, ~requestState, network);
};
