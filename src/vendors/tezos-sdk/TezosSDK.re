/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

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
    pkh: PublicKeyHash.t,
    pk_known: bool,
    sk_known: bool,
  };
};

module InputAddress = {
  type t = {
    alias: string,
    pkh: PublicKeyHash.t,
    force: bool,
  };
};

module Error = {
  type t =
    | BadPkh
    | Generic(string);

  type error_payload = {msg: string};

  let badPkh = "(Signature.Public_key_hash)";

  let parse = e => {
    e.msg
    ->(
        fun
        | s when s->Js.String2.includes(badPkh) => BadPkh
        | s => Generic(s)
      );
  };
};

type result('a) = {
  kind: [ | `ok | `error],
  payload: 'a,
  error_payload: Error.error_payload,
};

let fromPromise = p =>
  Future.make(resolve => {
    p
    |> Js.Promise.then_(v => {
         v.kind == `ok
           ? resolve(Ok(v.payload))
           : {
             Js.log(v);
             resolve(Error(v.error_payload->Error.parse));
           };
         Js.Promise.resolve();
       })
    |> Js.Promise.catch(error => {
         resolve(Error(Js.String.make(error)->Generic));
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
  "addAddress";
let addAddress = (sdk, alias, pkh) =>
  addAddress(sdk.lib, sdk.cctxt, {alias, pkh, force: true}) |> fromPromise;

type forgetParams = {
  name: string,
  force: bool,
};

[@bs.send]
external forgetAddress:
  (lib, cctxt, forgetParams) => Js.Promise.t(result(unit)) =
  "forgetAddress";
let forgetAddress = (sdk, name) =>
  forgetAddress(sdk.lib, sdk.cctxt, {name, force: true}) |> fromPromise;

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

type importSecretKeyParams = {
  name: string,
  force: bool,
  sk_uri: string,
};

[@bs.send]
external importSecretKey:
  (lib, cctxt, importSecretKeyParams, unit => string) =>
  Js.Promise.t(result(PublicKeyHash.t)) =
  "importSecretKey";
let importSecretKey = (sdk, ~name, ~skUri, ~password, ()) =>
  importSecretKey(sdk.lib, sdk.cctxt, {name, sk_uri: skUri, force: true}, () =>
    password
  )
  |> fromPromise;

type importKeysFromMnemonicsParams = {
  name: string,
  force: bool,
  encrypt: bool,
  mnemonics: string,
  passphrase: string,
};

[@bs.send]
external importKeysFromMnemonics:
  (lib, cctxt, importKeysFromMnemonicsParams, unit => string) =>
  Js.Promise.t(result(PublicKeyHash.t)) =
  "importKeysFromMnemonics";
let importKeysFromMnemonics = (sdk, ~name, ~mnemonics, ~password, ()) =>
  importKeysFromMnemonics(
    sdk.lib,
    sdk.cctxt,
    {name, mnemonics, passphrase: "", encrypt: true, force: true},
    () =>
    password
  )
  |> fromPromise;
