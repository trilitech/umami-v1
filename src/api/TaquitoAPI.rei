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

module Rpc: {
  /* Retrieve the balance of given public key hash */
  let getBalance:
    (
      ReTaquito.endpoint,
      ~address: PublicKeyHash.t,
      ~params: ReTaquito.RPCClient.params=?,
      unit
    ) =>
    Promise.t(Tez.t);

  let getBlockHeader:
    ReTaquito.endpoint => Promise.t(ReTaquito.RPCClient.blockHeader);

  let getChainId: ReTaquito.endpoint => Promise.t(string);
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
    Promise.t(option(PublicKeyHash.t));

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
    Promise.t(ReTaquito.Toolkit.Operation.result);

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
      Promise.t(Protocol.Simulation.results);
  };
};

module Operations: {
  /* Generate a `confirmation` from an operation hash, which returns once the
     operation have been included. */
  let confirmation:
    (ReTaquito.endpoint, ~hash: string, ~blocks: int=?, unit) =>
    Promise.t(ReTaquito.Toolkit.Operation.confirmationResult);
};

module type ContractCache = {
  type t;
  type contract;
  let make: ReTaquito.Toolkit.toolkit => t;
  let findContract: (t, PublicKeyHash.t) => Promise.t(contract);
  let clear: t => unit;
};

module FA12Cache:
  ContractCache with type contract := ReTaquitoContracts.FA12.t;

module FA2Cache: ContractCache with type contract := ReTaquitoContracts.FA2.t;

module Tzip12Cache:
  ContractCache with
    type contract := ReTaquitoContracts.Tzip12Tzip16Contract.t;

module Transfer: {
  /* Generates a list of Taquito-compatible transfers from a list of
     Transfer.elt. The endpoint is necessary to prefetch some contracts that can
     be repeated multiple times. */
  let prepareTransfers:
    (list(Transfer.elt), ReTaquito.endpoint, PublicKeyHash.t) =>
    FutureBase.t(list(Promise.result(ReTaquito.Toolkit.transferParams)));

  /* Multi asset batch */
  let batch:
    (
      ~endpoint: ReTaquito.endpoint,
      ~baseDir: System.Path.t,
      ~source: Wallet.PkhAlias.t,
      ~transfers: (ReTaquito.endpoint, PublicKeyHash.t) =>
                  FutureBase.t(
                    list(Promise.result(ReTaquito.Toolkit.transferParams)),
                  ),
      ~signingIntent: Signer.intent,
      unit
    ) =>
    Promise.t(ReTaquito.Toolkit.Operation.result);

  module Estimate: {
    let batch:
      (
        ~endpoint: ReTaquito.endpoint,
        ~baseDir: System.Path.t,
        ~source: PublicKeyHash.t,
        ~customValues: array((option(int), option(int), option(int))),
        ~transfers: (ReTaquito.endpoint, PublicKeyHash.t) =>
                    FutureBase.t(
                      list(Promise.result(ReTaquito.Toolkit.transferParams)),
                    ),
        unit
      ) =>
      Promise.t(Protocol.Simulation.results);
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
    Promise.t(ReTaquitoSigner.signature);
};
