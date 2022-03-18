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

type t;

let publicKey: t => Promise.t(string);
let publicKeyHash: t => Promise.t(PublicKeyHash.t);
let secretKey: t => Promise.t(option(string));

type signature = {
  bytes: string,
  prefixSig: string,
  sbytes: string,
  [@bs.as "sig"]
  sig_: string,
};

let sign: (t, string) => Promise.t(signature);

module MemorySigner: {
  let create:
    (~secretKey: string, ~passphrase: string=?, unit) => Promise.t(t);
};

module EstimationSigner: {
  let create: (~publicKey: string, ~publicKeyHash: PublicKeyHash.t, unit) => t;
};

module LedgerSigner: {
  let create:
    (
      ReLedger.Transport.t,
      DerivationPath.t,
      PublicKeyHash.Scheme.t,
      ~prompt: bool
    ) =>
    t;
};
