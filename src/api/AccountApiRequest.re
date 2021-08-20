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
  let get = (~config, ()) =>
    WalletAPI.Accounts.get(~config)
    ->Future.map(
        fun
        | Ok(response) =>
          response
          ->Array.map(((name, address)) =>
              ((address :> string), Account.{name, address})
            )
          ->Array.reverse
          ->Map.String.fromArray
          ->Ok
        | Error(System.NoSuchFileError(_)) => Map.String.empty->Ok
        | Error(_) as e => e,
      );

  ApiRequest.useLoader(
    ~get,
    ~kind=Logs.Account,
    ~errorToString=Errors.toString,
    ~requestState,
    (),
  );
};

/* Set */

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (~config, renaming: WalletAPI.Aliases.renameParams) =>
        WalletAPI.Aliases.rename(~config, renaming),
    ~kind=Logs.Account,
    ~errorToString=Errors.toString,
  );

let useDelete =
  ApiRequest.useSetter(
    ~set=WalletAPI.Accounts.delete,
    ~kind=Logs.Account,
    ~errorToString=Errors.toString,
  );

/* Other */

let useGetPublicKey = () => {
  let config = ConfigContext.useContent();

  (account: Account.t) => WalletAPI.Accounts.getPublicKey(~config, ~account);
};
