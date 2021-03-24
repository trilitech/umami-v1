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

let dummySdk = {lib: (Obj.magic(""): lib), cctxt: Obj.magic("")};

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

module InputAddress = {
  type t = {
    alias: string,
    pkh: string,
    force: bool,
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
  "listKnownAddresses";
let listKnownAddresses = sdk => {
  listKnownAddresses(sdk.lib, sdk.cctxt, 0) |> fromPromise;
};

[@bs.send]
external addAddress:
  (lib, cctxt, InputAddress.t) => Js.Promise.t(result(unit)) =
  "add_address";
let addAddress = (sdk, alias, pkh) =>
  addAddress(sdk.lib, sdk.cctxt, {alias, pkh, force: true}) |> fromPromise;

type renameParams = {
  old_name: string,
  new_name: string,
};

[@bs.send]
external renameAliases:
  (lib, cctxt, renameParams) => Js.Promise.t(result(unit)) =
  "renameAliases";

let renameAliases = (sdk, renameAlias) =>
  renameAliases(sdk.lib, sdk.cctxt, renameAlias) |> fromPromise;

[@bs.send]
external currentLevel: (lib, cctxt, int) => Js.Promise.t(result(int)) =
  "currentLevel";

let currentLevel = sdk => currentLevel(sdk.lib, sdk.cctxt, 0) |> fromPromise;
