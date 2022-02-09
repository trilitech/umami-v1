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
open ProtocolOptions;

type delegation = {
  source: Account.t,
  delegate: option(PublicKeyHash.t),
  options: delegationOptions,
};

type origination = {
  source: Account.t,
  balance: option(Tez.t),
  code: ReTaquitoTypes.Code.t,
  storage: ReTaquitoTypes.Storage.t,
  delegate: option(PublicKeyHash.t),
  options: originationOptions,
};

type t =
  | Delegation(delegation)
  | Origination(origination)
  | Transaction(Transfer.t);

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  source,
  delegate,
  options: makeDelegationOptions(~fee, ~burnCap, ~forceLowFee, ()),
};

let makeOrigination =
    (
      ~source,
      ~balance=?,
      ~code,
      ~storage,
      ~delegate: option(PublicKeyHash.t),
      ~fee=?,
      ~burnCap=?,
      ~forceLowFee=?,
      (),
    ) => {
  source,
  balance,
  delegate,
  code,
  storage,
  options: makeOriginationOptions(~fee, ~burnCap, ~forceLowFee, ()),
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

  let sumFees = a =>
    a->Array.reduce(Tez.zero, (acc, sim) => Tez.Infix.(acc + sim.fee));

  let computeRevealFees = sim =>
    sim.revealSimulation->Option.mapWithDefault(Tez.zero, ({fee}) => fee);

  let getTotalFees = sim =>
    Tez.Infix.(sim->computeRevealFees + sim.simulations->sumFees);
};

let optionsSet =
  fun
  | Transaction({transfers: [t]}) =>
    ProtocolOptions.txOptionsSet(t.tx_options)->Some
  | Transaction({transfers: _}) => None
  | Delegation(d) => ProtocolOptions.delegationOptionsSet(d.options)->Some
  | Origination(o) => ProtocolOptions.originationOptionsSet(o.options)->Some;

let isContractCall = (o, index) =>
  switch (o) {
  | Delegation(_) => false
  | Transaction((t: Transfer.t)) =>
    t.transfers
    ->List.get(index)
    ->Option.mapWithDefault(false, t =>
        t.amount->Transfer.Amount.getToken != None
        || t.destination->PublicKeyHash.isContract
      )
  | Origination(_) => false
  };
