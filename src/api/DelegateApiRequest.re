include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosClient, API.TezosExplorer);

/* Get delegate */

let useGetDelegate = account => {
  let network = StoreContext.useNetwork();

  let setAccountDelegate = StoreContext.useSetAccountDelegate();

  let get = (~config, (network, account: Account.t)) => {
    (network, config)
    ->DelegateAPI.getForAccount(account.address)
    ->Future.tapOk(res => setAccountDelegate(account.address, res));
  };

  let (getRequest, request) = ApiRequest.useGetter(get, Error.Delegate);

  React.useEffect2(
    () => {
      getRequest((network, account));

      None;
    },
    (network, account),
  );

  request;
};

/* Get Bakers */

let useGetBakers = () => {
  let network = StoreContext.useNetwork();

  let get = (~config as _c, network) => {
    DelegateAPI.getBakers(network);
  };

  let (getRequest, request) = ApiRequest.useGetter(get, Error.Delegate);

  React.useEffect1(
    () => {
      getRequest(network);
      None;
    },
    [|network|],
  );

  request;
};
