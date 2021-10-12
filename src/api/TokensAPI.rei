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

type Errors.t +=
  | NotFAContract(string);

let registeredTokens: unit => Let.result(TokenRegistry.Cache.t);

let addToken: (ConfigContext.env, Token.t) => Promise.t(unit);

let removeToken: (Token.t, ~pruneCache: bool) => Let.result(unit);

let fetchTokenContracts:
  (
    ConfigContext.env,
    ~accounts: list(PublicKeyHash.t),
    ~kinds: list(TokenContract.kind),
    ~limit: int64,
    ~index: int64
  ) =>
  Promise.t(array(TokenContract.t));

let fetchTokenRegistry:
  (
    ConfigContext.env,
    ~kinds: list(TokenContract.kind),
    ~limit: int64,
    ~index: int64
  ) =>
  Promise.t(array(TokenContract.t));
