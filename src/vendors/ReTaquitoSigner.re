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
"
const { LedgerSigner, DerivationType } = require('@taquito/ledger-signer');
const { InMemorySigner, importKey } = require('@taquito/signer');
";

type t;

[@bs.send] external publicKeyRaw: t => Js.Promise.t(string) = "publicKey";
[@bs.send]
external publicKeyHashRaw: t => Js.Promise.t(string) = "publicKeyHash";
[@bs.send]
external secretKeyRaw: t => Js.Promise.t(Js.Nullable.t(string)) =
  "secretKey";

let publicKey = t => t->publicKeyRaw->ReTaquitoError.fromPromiseParsed;
let publicKeyHash = t => t->publicKeyHashRaw->ReTaquitoError.fromPromiseParsed;
let secretKey = t =>
  t
  ->secretKeyRaw
  ->ReTaquitoError.fromPromiseParsed
  ->Future.mapOk(Js.Nullable.toOption);

module MemorySigner = {
  [@bs.val] [@bs.scope "InMemorySigner"]
  external fromSecretKey:
    (string, ~passphrase: string=?, unit) => Js.Promise.t(t) =
    "fromSecretKey";

  let create = (~secretKey, ~passphrase=?, ()) =>
    fromSecretKey(secretKey, ~passphrase?, ())
    ->ReTaquitoError.fromPromiseParsed;
};

module EstimationSigner = {
  %raw
  "
  class NoopSigner {
    constructor(pk, pkh) {
      this.pk = pk;
      this.pkh = pkh;
    }
    async publicKey() {
      return this.pk;
    }
    async publicKeyHash() {
      return this.pkh;
    }
    async secretKey() {
      throw new UnconfiguredSignerError();
    }
    async sign(_bytes, _watermark) {
      throw new UnconfiguredSignerError();
    }
  }
     ";

  [@bs.new]
  external create: (~publicKey: string, ~publicKeyHash: string, unit) => t =
    "NoopSigner";
};
