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

let convertLedgerError = res =>
  res->ResultEx.mapError(e => e->Wallet.convertLedgerError);

let convertWalletError = res =>
  res->ResultEx.mapError(e => ErrorHandler.Wallet(e));

let convertTaquitoError = res =>
  res->ResultEx.mapError(e => ErrorHandler.Taquito(e));

let mapError = (ft, constr) =>
  ft->Future.mapError(
    fun
    | ReTaquitoError.Generic(s) => constr(s)
    | e => e,
  );

let init = () => {
  ReLedger.Transport.create()
  ->ReTaquitoError.fromPromiseParsed
  ->mapError(s => s->ReTaquitoError.LedgerInit)
  ->Future.map(convertTaquitoError);
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
  ->Future.map(
      fun
      | Error(_) => Error(ReTaquitoError.LedgerNotReady->ErrorHandler.Taquito)
      | Ok(pk) =>
        // a valid pk is at 54 characters long, and 55 for tz3
        pk->Js.String2.startsWith("edpk") && pk->Js.String.length >= 54
          ? Ok() : Error(ReTaquitoError.LedgerNotReady->ErrorHandler.Taquito),
    );
};

module Signer = {
  type t = ReTaquitoSigner.t;

  let create = (tr, path, scheme, ~prompt) =>
    tr
    ->isReady
    ->Future.mapOk(() => tr->LedgerSigner.create(path, scheme, ~prompt));

  let publicKeyHash = signer =>
    signer->ReTaquitoSigner.publicKeyHash->Future.map(convertTaquitoError);

  let publicKey = signer =>
    signer->ReTaquitoSigner.publicKey->Future.map(convertTaquitoError);
};

let getKey = (~prompt, tr, path, schema) => {
  let signer = Signer.create(tr, path, schema, ~prompt);
  signer->Future.flatMapOk(Signer.publicKeyHash);
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
    (~ledgerTransport, ~dirpath, ~alias, ~path, ~scheme, ~ledgerBasePkh) => {
  let values = {
    let signer = Signer.create(ledgerTransport, path, ~prompt=false, scheme);
    /* Ensures the three are available */
    signer
    ->Future.flatMapOk(Signer.publicKeyHash)
    ->Future.flatMapOk(pkh => {
        signer
        ->Future.flatMapOk(Signer.publicKey)
        ->Future.flatMapOk(pk => {
            let sk =
              path
              ->DerivationPath.convertToTezosBip44
              ->Result.map(path => Wallet.Ledger.{path, scheme})
              ->Result.map(t =>
                  Wallet.Ledger.Encode.toSecretKey(t, ~ledgerBasePkh)
                )
              ->ResultEx.mapError(e => e->Wallet.Ledger.DerivationPathError)
              ->convertLedgerError
              ->convertWalletError;
            sk
            ->Result.map(sk => (sk, Wallet.ledgerPkValue(sk, pk), pkh))
            ->Future.value;
          })
      });
    /* sk->Future.tapOk(sk => Wallet.addAliasSk(~dirpath, ~alias, ~sk, ())); */
  };
  values->Future.flatMapOk(((sk, pk, pkh)) => {
    Wallet.addOrReplaceAlias(~dirpath, ~alias, ~pk, ~pkh, ~sk, ())
    ->Future.map(convertWalletError)
    ->Future.mapOk(() => pkh)
  });
};
