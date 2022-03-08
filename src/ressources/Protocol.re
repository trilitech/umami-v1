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

/** Protocol specific operations */
module Options = ProtocolOptions;
module Amount = ProtocolAmount;

module Transfer = {
  type generic('a) = {
    destination: PublicKeyHash.t,
    amount: 'a,
  };

  type transferFA2 = {
    tokenId: int,
    content: generic(Amount.token),
  };

  type batchFA2 = {
    address: PublicKeyHash.t,
    transfers: list(transferFA2),
  };

  type data =
    | FA2Batch(batchFA2)
    | Simple(generic(Amount.t));

  type t = {
    data,
    options: Options.t,
    parameter: Options.parameter,
  };
};

module Origination = {
  type t = {
    balance: option(Tez.t),
    code: ReTaquitoTypes.Code.t,
    storage: ReTaquitoTypes.Storage.t,
    delegate: option(PublicKeyHash.t),
    options: Options.t,
  };
};

module Delegation = {
  type t = {
    delegate: option(PublicKeyHash.t),
    options: Options.t,
  };
};

type manager =
  | Delegation(Delegation.t)
  | Transfer(Transfer.t)
  | Origination(Origination.t);

type batch = {
  source: Account.t,
  managers: array(manager),
};

module Simulation = {
  type resultElt = {
    fee: Tez.t,
    gasLimit: int,
    storageLimit: int,
  };

  type results = {
    simulations: array(resultElt),
    revealSimulation: option(resultElt),
  };
};
