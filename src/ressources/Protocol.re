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
  delegate: option(PublicKeyHash.t),
  fee: option(Tez.t),
};

type manager =
  | Delegation(delegation)
  | Transaction(Transfer.t);

type batch = {
  source: Account.t,
  managers: array(manager),
  options: operationOptions,
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
  | Transaction(t) => ProtocolOptions.txOptionsSet(t.options)->Some
  | Delegation(d) => Some(d.fee != None);

let isContractCall = o =>
  switch (o) {
  | Delegation(_) => false
  | Transaction((t: Transfer.t)) =>
    switch (t.data) {
    | Simple(data) =>
      data.destination->PublicKeyHash.isContract
      || data.amount->Transfer.Amount.getToken != None
    | FA2Batch(_) => true
    }
  };
