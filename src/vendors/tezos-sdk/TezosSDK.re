%raw
"var dummySDK;";

type sdkjs;

%raw
"
const initjs = require('tezos-sdk');
";

/* type sdk = Js.t(sdkjs); */
type cctxt;
type lib;
type sdkInit;
type t = {
  lib,
  cctxt,
};

let sdkInit: sdkInit = [%raw "initjs"];

[@bs.send] external buildCctxt: (lib, string, string) => cctxt = "buildCctxt";
[@bs.send] external init: sdkInit => Js.Promise.t(lib) = "init";

let libPromise = init(sdkInit);

let init: (string, string) => Js.Promise.t(t) =
  (baseDir, endpoint) =>
    libPromise
    |> Js.Promise.then_(lib => {
         Js.Promise.resolve({lib, cctxt: buildCctxt(lib, baseDir, endpoint)})
       });

module OutputAddress = {
  type t = {
    alias: string,
    pkh: string,
    pk_known: bool,
    sk_known: bool,
  };
};

type result('a) = {
  kind: [ | `ok | `error],
  payload: 'a,
  msg: string,
};

let fromPromise = p =>
  Future.make(resolve => {
    p
    |> Js.Promise.then_(v => {
         v.kind == `ok
           ? resolve(Ok(v.payload))
           : {
             resolve(Error(v.msg));
           };
         Js.Promise.resolve();
       })
    |> Js.Promise.catch(error => {
         resolve(Error(Js.String.make(error)));
         Js.Promise.resolve();
       })
  });

[@bs.send]
external listKnownAddresses:
  (lib, cctxt, int) => Js.Promise.t(result(array(OutputAddress.t))) =
  "list_known_addresses";
let listKnownAddresses = sdk =>
  sdk
  |> Js.Promise.then_(sdk => listKnownAddresses(sdk.lib, sdk.cctxt, 0))
  |> fromPromise;
