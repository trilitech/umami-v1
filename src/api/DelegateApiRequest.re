include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosExplorer);

/* Get delegate */

let useLoad = (~requestState, ~address: string) => {
  let get = (~settings, address) =>
    DelegateAPI.getForAccount(settings, address);

  ApiRequest.useLoader1(~get, ~kind=Logs.Delegate, ~requestState, address);
};

/* Get delegate info */

let useLoadInfo = (~requestState, ~address: string) => {
  let get = (~settings, address) =>
    DelegateAPI.getDelegationInfoForAccount(settings, address);

  ApiRequest.useLoader1(~get, ~kind=Logs.Delegate, ~requestState, address);
};

/* Get Bakers */

let useLoadBakers = (~requestState) => {
  let get = (~settings, ()) => DelegateAPI.getBakers(settings);

  ApiRequest.useLoader(~get, ~kind=Logs.Delegate, ~requestState, ());
};
