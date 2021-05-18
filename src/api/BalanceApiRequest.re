module BalanceAPI = API.Balance;

type balanceApiRequest = ApiRequest.t(string, string);

let useLoad = (~requestState as (request, setRequest), ~address: string) => {
  let get = (~settings, address) => {
    settings->BalanceAPI.get(address, ());
  };

  let getRequest =
    ApiRequest.useGetter(
      ~get,
      ~kind=Logs.Balance,
      ~setRequest,
      ~errorToString=x => x,
      (),
    );

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect3(
    () => {
      let shouldReload = ApiRequest.conditionToLoad(request, isMounted);
      if (address != "" && shouldReload) {
        getRequest(address)->ignore;
      };
      None;
    },
    (isMounted, request, address),
  );

  request;
};
