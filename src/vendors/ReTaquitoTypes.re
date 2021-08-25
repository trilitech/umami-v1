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

module BigNumber: {
  type fixed;
  let toFixed: ReBigNumber.t => fixed;
  let fromInt64: Int64.t => ReBigNumber.t;
  let toInt64: ReBigNumber.t => Int64.t;
} = {
  type fixed = string;

  let toFixed = ReBigNumber.toFixed;

  let fromInt64 = ReBigNumber.fromInt64;
  let toInt64 = ReBigNumber.toInt64;
};

module RPCClient = {
  type t;

  type params = {block: string};
  type managerKeyResult = {key: string};
};

module Operation = {
  type field;
  type t;

  type block = {hash: string};

  type confirmationResult = {block};

  type result = {hash: string};
};

module Toolkit = {
  type tz;
  type contract;
  type estimate;

  type toolkit = {
    tz,
    contract,
    operation: Operation.field,
    estimate,
  };

  type provider = {signer: ReTaquitoSigner.t};
};

module Transfer = {
  type transferParams = {
    kind: string,
    [@bs.as "to"]
    to_: PublicKeyHash.t,
    source: PublicKeyHash.t,
    amount: ReBigNumber.t,
    fee: option(ReBigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
    mutez: option(bool),
    parameter: option(ProtocolOptions.TransactionParameters.t),
  };

  type sendParams = {
    amount: ReBigNumber.t,
    fee: option(ReBigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
    mutez: option(bool),
  };
};

module Delegate = {
  type delegateParams = {
    source: PublicKeyHash.t,
    delegate: option(PublicKeyHash.t),
    fee: option(ReBigNumber.t),
  };
};

module Estimation = {
  type result = {
    totalCost: int,
    storageLimit: int,
    gasLimit: int,
    revealFee: int,
    minimalFeeMutez: int,
    suggestedFeeMutez: int,
    burnFeeMutez: int,
    customFeeMutez: int,
  };
};

module Batch = {
  type t;
};

module Contract = {
  type methodResult('meth);
};

/* Abstraction of a kind of contract, see `ReTaquitoContracts.FA12Abstraction`
   for a good example. */
module type ContractAbstraction = {
  /* This corresponds to ContractAbstraction for the TaquitoAPI */

  /* Abstract storage representation */
  type storage;
  /* Entrypoints representation in Michelson */
  type entrypoints;
  /* Concrete entrypoints calls  */
  type methods;

  /* Taquito's `ContractAbstraction` object, with only the necessary parts. */
  type t = {
    address: PublicKeyHash.t,
    entrypoints,
    methods,
  };
};
