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

let handleEstimationResults:
  (
    (array(ReTaquito.Toolkit.Estimation.result), int),
    array((option(int), option(int), option(int))),
    option(int)
  ) =>
  Result.t(ReTaquito.Toolkit.Estimation.result, Errors.t);

module Balance: {
  /* Retrieve the balance of given public key hash */
  let get:
    (
      ReTaquito.endpoint,
      ~address: PublicKeyHash.t,
      ~params: ReTaquito.RPCClient.params=?,
      unit
    ) =>
    Future.t(Result.t(Tez.t, Errors.t));
};

module Signer: {
  type intent =
    | LedgerCallback(unit => unit)
    | Password(string);
};

module Delegate: {
  /* Retrieve the delegate of a given public key hash, returns None if the
     account is not delegated */
  let get:
    (ReTaquito.endpoint, PublicKeyHash.t) =>
    Future.t(Result.t(option(PublicKeyHash.t), Errors.t));

  /* Set the delegate for a given account */
  let set:
    (
      ~endpoint: ReTaquito.endpoint,
      ~baseDir: System.Path.t,
      ~source: PublicKeyHash.t,
      ~delegate: option(PublicKeyHash.t),
      ~signingIntent: Signer.intent,
      ~fee: Tez.t=?,
      unit
    ) =>
    Future.t(Result.t(ReTaquito.Toolkit.Operation.result, Errors.t));

  module Estimate: {
    let set:
      (
        ~endpoint: ReTaquito.endpoint,
        ~baseDir: System.Path.t,
        ~source: PublicKeyHash.t,
        ~delegate: PublicKeyHash.t=?,
        ~fee: Tez.t=?,
        unit
      ) =>
      Future.t(Result.t(ReTaquito.Toolkit.Estimation.result, Errors.t));
  };
};

module Operations: {
  /* Generate a `confirmation` from an operation hash, which returns once the
     operation have been included. */
  let confirmation:
    (ReTaquito.endpoint, ~hash: string, ~blocks: int=?, unit) =>
    Future.t(
      Result.t(ReTaquito.Toolkit.Operation.confirmationResult, Errors.t),
    );
};

module Transfer: {
  /* Generates a list of Taquito-compatible transfers from a list of
     Transfer.elt. The endpoint is necessary to prefetch some contracts that can
     be repeated multiple times. */
  let prepareTransfers:
    (list(Transfer.elt), ReTaquito.endpoint, PublicKeyHash.t) =>
    Future.t(list(Result.t(ReTaquito.Toolkit.transferParams, Errors.t)));

  /* Multi asset batch */
  let batch:
    (
      ~endpoint: ReTaquito.endpoint,
      ~baseDir: System.Path.t,
      ~source: Wallet.PkhAlias.t,
      ~transfers: (ReTaquito.endpoint, PublicKeyHash.t) =>
                  Future.t(
                    list(
                      Result.t(ReTaquito.Toolkit.transferParams, Errors.t),
                    ),
                  ),
      ~signingIntent: Signer.intent,
      unit
    ) =>
    Future.t(Result.t(ReTaquito.Toolkit.Operation.result, Errors.t));

  module Estimate: {
    let batch:
      (
        ~endpoint: ReTaquito.endpoint,
        ~baseDir: System.Path.t,
        ~source: PublicKeyHash.t,
        ~transfers: (ReTaquito.endpoint, PublicKeyHash.t) =>
                    Future.t(
                      list(
                        Result.t(ReTaquito.Toolkit.transferParams, Errors.t),
                      ),
                    ),
        unit
      ) =>
      Future.t(
        Result.t(
          (array(ReTaquito.Toolkit.Estimation.result), int),
          Errors.t,
        ),
      );
  };
};

module Signature: {
  let signPayload:
    (
      ~baseDir: System.Path.t,
      ~source: Wallet.PkhAlias.t,
      ~signingIntent: Signer.intent,
      ~payload: string
    ) =>
    Future.t(Result.t(ReTaquitoSigner.signature, Errors.t));
};
