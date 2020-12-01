include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosClient, API.TezosExplorer);

/* Get delegate */

let useLoad =
    (~network, ~requestState as (request, setRequest), ~address: string) => {
  let get = (~config, network, address) =>
    DelegateAPI.getForAccount((network, config), address);

  ApiRequest.useStoreLoader2(
    ~get,
    ~kind=Logs.Delegate,
    ~request,
    ~setRequest,
    network,
    address,
  );

  request;
};

/* Get delegate info */

let useLoadInfo =
    (~network, ~requestState as (request, setRequest), ~address: string) => {
  let get = (~config, network, address) =>
    DelegateAPI.getDelegationInfoForAccount((network, config), address);

  ApiRequest.useStoreLoader2(
    ~get,
    ~kind=Logs.Delegate,
    ~request,
    ~setRequest,
    network,
    address,
  );

  request;
};

/* Get Bakers */

let useLoadBakers = (~network, ~requestState as (request, setRequest)) => {
  let get = (~config as _c, network) => DelegateAPI.getBakers(network);

  ApiRequest.useStoreLoader1(
    ~get,
    ~kind=Logs.Delegate,
    ~request,
    ~setRequest,
    network,
  );

  request;
};
