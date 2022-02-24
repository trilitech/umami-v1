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

/*
 A custom auth signer allows to sign with providers credentials
 and read the secret key.

 It follows those steps:
 - init torus sdk
 - fetches keys from Torus.

 */

type Errors.t +=
  | HandleMismatch(string, string);

let () =
  Errors.registerHandler(
    "CustomAuthSigner",
    fun
    | HandleMismatch(s, s') =>
      I18n.Errors.customauth_handle_mismatch(s, s')->Some
    | _ => None,
  );

open ReCustomAuth;

open Let;
type publicKey = string;

type t = ReTaquitoSigner.t;
let toSigner = x => x;

let create = (~accountHandle=?, provider) => {
  let%Await () = torusSdk->init(initParams(~skipSw=true, ()));

  let%Await res = torusSdk->triggerAggregateLogin(~accountHandle?, provider);

  let sk =
    ReTaquitoUtils.b58cencode(
      (res.privateKey :> string),
      ReTaquitoUtils.prefix.spsk,
    );
  let%AwaitRes signer =
    ReTaquitoSigner.MemorySigner.create(~secretKey=sk, ());
  let%ResMap info = res.userInfo->findInfo(provider);

  (signer, info);
};

let getInfos = provider => {
  let%Await (signer, userInfo) = create(provider);
  let%Await pkh = signer->ReTaquitoSigner.publicKeyHash;
  let%AwaitRes pk = signer->ReTaquitoSigner.publicKey;
  let%ResMap handle =
    ReCustomAuth.Handle.resolve(~email=userInfo.email, ~name=userInfo.name);
  (pkh, pk, {handle, provider});
};

let create = infos => {
  let%AwaitRes (signer, signingInfos) =
    create(~accountHandle=infos.handle, infos.provider);

  let%Res signingHandle =
    ReCustomAuth.Handle.resolve(
      ~email=signingInfos.email,
      ~name=signingInfos.name,
    );

  infos.handle != signingHandle
    ? Error(
        HandleMismatch((infos.handle :> string), (signingHandle :> string)),
      )
    : Ok(signer);
};
