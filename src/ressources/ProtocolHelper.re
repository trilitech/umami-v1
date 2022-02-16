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

open ProtocolOptions;
open Protocol;

module Delegation = {
  let makeSingleton =
      (
        ~source,
        ~infos as Delegation.{delegate, fee},
        ~burnCap=?,
        ~forceLowFee=?,
        (),
      ) => {
    let options = makeOperationOptions(~burnCap?, ~forceLowFee?, ());
    Protocol.{source, managers: [|{delegate, fee}->Delegation|], options};
  };
};

module Transfer = {
  open Protocol.Transfer;

  let makeSimple =
      (
        ~data,
        ~fee=?,
        ~parameter=?,
        ~entrypoint=?,
        ~gasLimit=?,
        ~storageLimit=?,
        (),
      ) => {
    data: Simple(data),
    options:
      makeTransferEltOptions(
        ~fee?,
        ~gasLimit?,
        ~storageLimit?,
        ~parameter?,
        ~entrypoint?,
        (),
      ),
  };

  let makeSimpleTez = (~destination, ~amount) =>
    makeSimple(~data={destination, amount: Amount.makeTez(amount)});

  /* Tokens cannot define parameter and entrypoint, since they are
     already translated as parameters into an entrypoint */
  let makeSimpleToken =
      (
        ~destination,
        ~amount,
        ~token,
        ~fee=?,
        ~gasLimit=?,
        ~storageLimit=?,
        (),
      ) =>
    makeSimple(
      ~data={destination, amount: Amount.makeToken(~amount, ~token)},
      ~fee?,
      ~gasLimit?,
      ~storageLimit?,
      (),
    );

  let makeBatch = (~source, ~transfers, ~burnCap=?, ~forceLowFee=?, ()) => {
    let options = makeOperationOptions(~burnCap?, ~forceLowFee?, ());
    {source, managers: transfers->Array.map(t => Transfer(t)), options};
  };

  let makeSingleton =
      (
        ~source,
        ~amount,
        ~destination,
        ~burnCap=?,
        ~forceLowFee=?,
        ~fee=?,
        ~parameter=?,
        ~entrypoint=?,
        ~gasLimit=?,
        ~storageLimit=?,
        (),
      ) => {
    let transfers = [|
      makeSimpleTez(
        ~amount,
        ~destination,
        ~fee?,
        ~parameter?,
        ~entrypoint?,
        ~gasLimit?,
        ~storageLimit?,
        (),
      ),
    |];

    makeBatch(~source, ~transfers, ~burnCap?, ~forceLowFee?, ());
  };

  let reduceArray = (transfers, f) =>
    transfers->Array.reduce([], (acc, t: t) =>
      switch (t.data) {
      | Simple(t) => f(acc, t)
      | FA2Batch({transfers}) =>
        transfers->List.reduce(acc, (acc, {content}) =>
          f(
            acc,
            {
              destination: content.destination,
              amount: Amount.Token(content.amount),
            },
          )
        )
      }
    );
};

let optionsSet =
  fun
  | Transfer(t) => ProtocolOptions.txOptionsSet(t.options)->Some
  | Delegation(d) => Some(d.fee != None);

let isContractCall = o =>
  switch (o) {
  | Delegation(_) => false
  | Transfer((t: Protocol.Transfer.t)) =>
    switch (t.data) {
    | Simple(data) =>
      data.destination->PublicKeyHash.isContract
      || data.amount->Protocol.Amount.getToken != None
    | FA2Batch(_) => true
    }
  };

module Simulation = {
  open Protocol.Simulation;
  let sumFees = a =>
    a->Array.reduce(Tez.zero, (acc, sim) => Tez.Infix.(acc + sim.fee));

  let computeRevealFees = sim =>
    sim.revealSimulation->Option.mapWithDefault(Tez.zero, ({fee}) => fee);

  let getTotalFees = sim =>
    Tez.Infix.(sim->computeRevealFees + sim.simulations->sumFees);
};
