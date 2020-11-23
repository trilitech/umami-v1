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
