include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosClient, API.TezosExplorer);

/* Get delegate */

let useGetDelegate = (account: Account.t) => {
  let network = StoreContext.useNetwork();
  let setAccountDelegate = StoreContext.useSetAccountDelegate();

  ApiRequest.useLoader2(
    (~config, network, address) =>
      DelegateAPI.getForAccount((network, config), address)
      ->Future.tapOk(res => setAccountDelegate(address, res)),
    Error.Delegate,
    network,
    account.address,
  );
};

/* Get delegate info */

let useGetDelegateInfo = (account: Account.t) => {
  let network = StoreContext.useNetwork();

  ApiRequest.useLoader2(
    (~config, network, address) =>
      DelegateAPI.getDelegationInfoForAccount((network, config), address),
    Error.Delegate,
    network,
    account.address,
  );
};

/* Get Bakers */

let useGetBakers = () => {
  let network = StoreContext.useNetwork();

  ApiRequest.useLoader1(
    (~config as _c, network) => DelegateAPI.getBakers(network),
    Error.Delegate,
    network,
  );
};
