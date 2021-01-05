module BalanceAPI = API.Balance(API.TezosClient);

type balanceApiRequest = ApiRequest.t(string);

let useLoad = (~requestState as (request, setRequest), ~address: string) => {
  let get = (~settings, address) => {
    settings->BalanceAPI.get(address, ());
  };

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Balance, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect3(
    () => {
      let shouldReload = ApiRequest.conditionToLoad(request, isMounted);
      if (address != "" && shouldReload) {
        getRequest(address);
      };
      None;
    },
    (isMounted, request, address),
  );

  request;
};
