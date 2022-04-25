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

type publicKey = string;

type t = ReTaquitoSigner.t;
let toSigner = x => x;

let create = (~accountHandle=?, provider) => {
  torusSdk
  ->init(initParams(~skipSw=true, ()))
  ->Promise.flatMapOk(() =>
      torusSdk->triggerAggregateLogin(~accountHandle?, provider)
    )
  ->Promise.flatMapOk(res => {
      let sk =
        ReTaquitoUtils.b58cencode(
          (res.privateKey :> string),
          ReTaquitoUtils.prefix.spsk,
        );
      ReTaquitoSigner.MemorySigner.create(~secretKey=sk, ())
      ->Promise.flatMapOk(signer =>
          res.userInfo
          ->findInfo(provider)
          ->Result.map(info => (signer, info))
          ->Promise.value
        );
    });
};

let getInfos = provider => {
  create(provider)
  ->Promise.flatMapOk(((signer, userInfo)) => {
      let pkh = signer->ReTaquitoSigner.publicKeyHash;
      let pk = signer->ReTaquitoSigner.publicKey;

      Promise.flatMapOk2(pkh, pk, (pkh, pk) =>
        ReCustomAuth.Handle.resolve(
          ~email=userInfo.email,
          ~name=userInfo.name,
        )
        ->Result.map(handle => (pkh, pk, {handle, provider}))
        ->Promise.value
      );
    });
};

let create = infos => {
  create(~accountHandle=infos.handle, infos.provider)
  ->Promise.flatMapOk(((signer, signingInfos)) => {
      ReCustomAuth.Handle.resolve(
        ~email=signingInfos.email,
        ~name=signingInfos.name,
      )
      ->Result.flatMap(signingHandle =>
          infos.handle != signingHandle
            ? Error(
                HandleMismatch(
                  (infos.handle :> string),
                  (signingHandle :> string),
                ),
              )
            : Ok(signer)
        )
      ->Promise.value
    });
};
