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
  let toFixed: (~decimals: int=?, ReBigNumber.t) => fixed;
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

  type blockHeader = {
    protocol: string,
    chain_id: string,
    level: int,
  };

  type constants = {
    hard_gas_limit_per_operation: ReBigNumber.t,
    hard_storage_limit_per_operation: ReBigNumber.t,
  };
};

module Operation = {
  type kind;

  [@bs.module "@taquito/taquito"] [@bs.scope "OpKind"]
  external transactionKind: kind = "TRANSACTION";

  [@bs.module "@taquito/taquito"] [@bs.scope "OpKind"]
  external delegationKind: kind = "DELEGATION";

  [@bs.module "@taquito/taquito"] [@bs.scope "OpKind"]
  external originationKind: string = "ORIGINATION";

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

module Micheline: {
  type t = pri string;

  let unitVal: t;
} = {
  type t = string;

  let unitVal = "Unit";
};

module Code = {
  type t;
};

module Storage = {
  type t;
};

module Transfer = {
  module Entrypoint = {
    type name = string;
    type param = {
      entrypoint: name,
      value: Micheline.t,
    };

    let default = "default";
  };

  type transferParams = {
    kind: Operation.kind,
    [@bs.as "to"]
    to_: PublicKeyHash.t,
    source: PublicKeyHash.t,
    amount: ReBigNumber.t,
    fee: option(ReBigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
    mutez: option(bool),
    parameter: option(Entrypoint.param),
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
    kind: Operation.kind,
    source: PublicKeyHash.t,
    delegate: option(PublicKeyHash.t),
    fee: option(ReBigNumber.t),
  };
};

module Originate = {
  type originateParams = {
    kind: string,
    source: PublicKeyHash.t,
    balance: option(ReBigNumber.t),
    code: Code.t,
    [@bs.as "init"]
    storage: Storage.t,
    delegate: option(PublicKeyHash.t),
    fee: option(ReBigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
    mutez: option(bool),
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

module Tzip12 = {
  type metadata = {
    token_id: int,
    decimals: int,
    name: string,
    symbol: string,
    // TZIP21 Asset fields
    description: option(string),
    minter: option(PublicKeyHash.t),
    creators: option(array(string)),
    contributors: option(array(string)),
    publishers: option(array(string)),
    date: option(string),
    blocklevel: option(int),
    [@bs.as "type"]
    type_: option(string),
    tags: option(array(string)),
    genres: option(array(string)),
    language: option(string),
    identifier: option(string),
    rights: option(string),
    rightUri: option(string),
    artifactUri: option(string),
    displayUri: option(string),
    thumbnailUri: option(string),
    isTransferable: option(bool), // default: true
    isBooleanAmount: option(bool), // default: false
    shouldPreferSymbol: option(bool), //default: false
    formats: option(array(TokenRepr.Metadata.format)),
    attributes: option(array(TokenRepr.Metadata.attribute)),
  };

  type t = {getTokenMetadata: (. int) => Js.Promise.t(metadata)};
};

module Tzip16 = {
  type metadata = {name: option(string)};
  type metadataRes = {metadata};

  type t = {getMetadata: (. unit) => Js.Promise.t(metadataRes)};
};

module Contract = {
  /** Method results, which are generated functions from entrypoints:
     - `send`, that sends the result of an entrypoint to the contract
     - `toTransferParams`, that generates the transferParam from an entrypoint
   */
  type methodResult('meth) = {
    send: (. Transfer.sendParams) => Js.Promise.t(Operation.result),
    toTransferParams: (. Transfer.sendParams) => Transfer.transferParams,
  };
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

  /* Taquito's `ContractAbstraction` object. */
  type t;
};

module MapAbstraction = (Key: {type t;}) => {
  type key = Key.t;
  type t('a) = {get: (. key) => option('a)};
};

module BigMapAbstraction = (Key: {type t;}) => {
  type key = Key.t;
  type t('a) = {get: (. key) => Js.Promise.t(option('a))};
};

module Tzip12Storage = {
  module Fields =
    MapAbstraction({
      type t = string;
    });

  /* optional fields: undefined in the JS object */
  type token = {
    token_id: option(ReBigNumber.t),
    token_info: option(Fields.t(bytes)),
  };

  /* An unannotated michelson value is returned as a polymorphic array */
  type unannoted_token = {
    [@bs.as "0"]
    un_token_id: option(ReBigNumber.t),
    [@bs.as "1"]
    un_token_info: option(Fields.t(bytes)),
  };

  module Tokens = {
    include BigMapAbstraction({
      type t = ReBigNumber.t;
    });

    /* This ugly trick is unfortunately the only way to get an unannoted token
       when the storage is illformed, as we cannot give it another type. */
    [@bs.send]
    external getUnannotated:
      (t(token), key) => Js.Promise.t(option(unannoted_token)) =
      "get";
  };

  type storage = {
    // Many contracts put their metadata into a pair at toplevel called `assets`
    assets: option(storage),
    token_metadata: option(Tokens.t(token)),
  };
};

module Extension = {
  type t;

  type abstraction;
};

module Tzip16Contract = {
  type methods;
  type storage;
  type entrypoints;

  type t = {
    .
    "address": PublicKeyHash.t,
    "entrypoints": entrypoints,
    "methods": methods,
    [@bs.meth] "storage": unit => Js.Promise.t(storage),
    [@bs.meth] "tzip16": unit => Tzip16.t,
  };
};

module Tzip12Tzip16Contract = {
  type methods;
  type storage = Tzip12Storage.storage;
  type entrypoints;

  type t = {
    .
    "address": PublicKeyHash.t,
    "entrypoints": entrypoints,
    "methods": methods,
    [@bs.meth] "storage": unit => Js.Promise.t(storage),
    [@bs.meth] "tzip12": unit => Tzip12.t,
    [@bs.meth] "tzip16": unit => Tzip16.t,
  };
};

module FA12 = {
  type transfer;

  type methods = {
    transfer:
      (. PublicKeyHash.t, PublicKeyHash.t, BigNumber.fixed) =>
      Contract.methodResult(transfer),
  };
  type storage = Tzip12Storage.storage;
  type entrypoints;

  type t = {
    .
    "address": PublicKeyHash.t,
    "entrypoints": entrypoints,
    "methods": methods,
    [@bs.meth] "storage": unit => Js.Promise.t(storage),
    [@bs.meth] "tzip12": unit => Tzip12.t,
    [@bs.meth] "tzip16": unit => Tzip16.t,
  };
};

module FA2 = {
  type transaction = {
    to_: PublicKeyHash.t,
    token_id: BigNumber.fixed,
    amount: BigNumber.fixed,
  };
  type transferParam = {
    from_: PublicKeyHash.t,
    txs: array(transaction),
  };

  type transfer;

  /* Balance_of is never called directly, however it could be used in the future
     to generate the input for `run_view`. */
  type balanceOfRequest = {
    owner: PublicKeyHash.t,
    token_id: BigNumber.fixed,
  };

  type balanceOf;

  type methods = {
    transfer: (. array(transferParam)) => Contract.methodResult(transfer),
    balance_of:
      (. array(balanceOfRequest), PublicKeyHash.t) =>
      Contract.methodResult(balanceOf),
  };
  type storage = Tzip12Storage.storage;
  type entrypoints;

  type t = {
    .
    "address": PublicKeyHash.t,
    "entrypoints": entrypoints,
    "methods": methods,
    [@bs.meth] "storage": unit => Js.Promise.t(storage),
    [@bs.meth] "tzip12": unit => Tzip12.t,
    [@bs.meth] "tzip16": unit => Tzip16.t,
  };
};
