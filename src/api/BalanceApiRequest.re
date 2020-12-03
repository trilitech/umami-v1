module BalanceAPI = API.Balance(API.TezosClient);

type balanceApiRequest = ApiRequest.t(string);

let useLoad =
    (~network, ~requestState as (request, setRequest), ~address: string) => {
  let get = (~config, (network, address)) => {
    (network, config)->BalanceAPI.get(address, ());
  };

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Balance, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect4(
    () => {
      let (shouldReload, loading) =
        ApiRequest.conditionToLoad(request, isMounted);
      if (address != "" && shouldReload) {
        getRequest(~loading, (network, address));
      };
      None;
    },
    (isMounted, network, request, address),
  );

  request;
};
