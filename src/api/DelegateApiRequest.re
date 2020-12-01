include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosClient, API.TezosExplorer);

/* Get delegate */

let useGetDelegate = (account: Account.t) => {
  let network = StoreContext.useNetwork();

  let (request, setRequest) =
    StoreContext.useDelegateRequestState(Some(account.address));

  let get = (~config, network, address) =>
    DelegateAPI.getForAccount((network, config), address);

  ApiRequest.useStoreLoader2(
    ~get,
    ~kind=Logs.Delegate,
    ~request,
    ~setRequest,
    network,
    account.address,
  );

  request;
};

/* Get delegate info */

let useGetDelegateInfo = (account: Account.t) => {
  let network = StoreContext.useNetwork();

  ApiRequest.useLoader2(
    (~config, network, address) =>
      DelegateAPI.getDelegationInfoForAccount((network, config), address),
    Logs.Delegate,
    network,
    account.address,
  );
};

/* Get Bakers */

let useGetBakers = () => {
  let network = StoreContext.useNetwork();

  ApiRequest.useLoader1(
    (~config as _c, network) => DelegateAPI.getBakers(network),
    Logs.Delegate,
    network,
  );
};
