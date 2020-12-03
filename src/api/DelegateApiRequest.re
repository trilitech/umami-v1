include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosClient, API.TezosExplorer);

/* Get delegate */

let useLoad = (~network, ~requestState, ~address: string) => {
  let get = (~config, (network, address)) =>
    DelegateAPI.getForAccount((network, config), address);

  ApiRequest.useLoader2(
    ~get,
    ~kind=Logs.Delegate,
    ~requestState,
    network,
    address,
  );
};

/* Get delegate info */

let useLoadInfo = (~network, ~requestState, ~address: string) => {
  let get = (~config, (network, address)) =>
    DelegateAPI.getDelegationInfoForAccount((network, config), address);

  ApiRequest.useLoader2(
    ~get,
    ~kind=Logs.Delegate,
    ~requestState,
    network,
    address,
  );
};

/* Get Bakers */

let useLoadBakers = (~network, ~requestState) => {
  let get = (~config as _c, network) => DelegateAPI.getBakers(network);

  ApiRequest.useLoader1(~get, ~kind=Logs.Delegate, ~requestState, network);
};
