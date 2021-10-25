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

open ReTaquitoSigner;
open Let;

let init = (~timeout=?, ()) => {
  ReLedger.Transport.create(~listenTimeout=?timeout, ());
};

/* This current function is a hack until there is a proper way to know if a
   ledger is currently locked or not. */
let isReady = tr => {
  let signer =
    LedgerSigner.create(
      tr,
      Wallet.Ledger.masterKeyPath,
      Wallet.Ledger.masterKeyScheme,
      ~prompt=false,
    );
  signer
  ->ReTaquitoSigner.publicKey
  ->Promise.map(
      fun
      | Error(_) => Error(ReTaquitoError.LedgerNotReady)
      | Ok(pk) =>
        // a valid pk is at 54 characters long, and 55 for tz3
        pk->Js.String2.startsWith("edpk") && pk->Js.String.length >= 54
          ? Ok() : Error(ReTaquitoError.LedgerNotReady),
    );
};

module Signer = {
  type t = ReTaquitoSigner.t;

  let create = (tr, path, scheme, ~prompt) =>
    tr
    ->isReady
    ->Promise.mapOk(() => tr->LedgerSigner.create(path, scheme, ~prompt));

  let publicKeyHash = signer => signer->ReTaquitoSigner.publicKeyHash;

  let publicKey = signer => signer->ReTaquitoSigner.publicKey;
};

let getKey = (~prompt, tr, path, schema) => {
  let signer = Signer.create(tr, path, schema, ~prompt);
  signer->Promise.flatMapOk(Signer.publicKeyHash);
};

let getFirstKey = (~prompt, tr) => {
  let path = DerivationPath.Pattern.(default->fromTezosBip44->implement(0));
  getKey(~prompt, tr, path, Wallet.Ledger.masterKeyScheme);
};

let getMasterKey = (~prompt, tr) =>
  getKey(
    ~prompt,
    tr,
    Wallet.Ledger.masterKeyPath,
    Wallet.Ledger.masterKeyScheme,
  );

/* This function depends on LedgerSigner to compute the pk and pkh, hence it
   cannot be defined directly into Wallet.re */
let addOrReplaceAlias =
    (~ledgerTransport, ~dirpath, ~alias, ~path, ~scheme, ~ledgerBasePkh)
    : Promise.t(PublicKeyHash.t) => {
  let%Await signer =
    Signer.create(ledgerTransport, path, ~prompt=false, scheme);
  /* Ensures the three are available */

  let%Await pkh = signer->Signer.publicKeyHash;

  let%Await pk = signer->Signer.publicKey;

  let%Await path = path->DerivationPath.convertToTezosBip44->Promise.value;

  let t = Wallet.Ledger.{path, scheme};
  let sk = Wallet.Ledger.Encode.toSecretKey(t, ~ledgerBasePkh);

  let pk = Wallet.ledgerPkValue(sk, pk);

  let%AwaitMap () = Wallet.addOrReplaceAlias(~dirpath, ~alias, ~pk, ~pkh, ~sk);

  pkh;
};
