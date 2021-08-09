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

/* ACCOUNT */

/* Get */

let useLoad = requestState => {
  let get = (~settings, ()) =>
    WalletAPI.Accounts.get(~settings)
    ->Future.mapOk(response => {
        response
        ->Array.map(((name, address)) =>
            ((address :> string), Account.{name, address, ledger: false})
          )
        ->Array.reverse
        ->Map.String.fromArray
      });

  ApiRequest.useLoader(~get, ~kind=Logs.Account, ~requestState, ());
};

/* Set */

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (~settings, renaming: TezosSDK.renameParams) =>
        WalletAPI.Aliases.rename(~settings, renaming),
    ~kind=Logs.Account,
    ~errorToString=ErrorHandler.toString,
  );

let useDelete =
  ApiRequest.useSetter(
    ~set=WalletAPI.Accounts.delete,
    ~kind=Logs.Account,
    ~errorToString=ErrorHandler.toString,
  );

/* Other */

let useGetPublicKey = () => {
  let settings = SdkContext.useSettings();

  (account: Account.t) =>
    WalletAPI.Accounts.getPublicKey(~settings, ~account);
};
