/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

module Options = ProtocolOptions
open Protocol

module Delegation = {
  let makeSingleton = (~source, ~infos as {Delegation.delegate: delegate, options}, ()) => {
    open Protocol
    {source: source, managers: [{delegate: delegate, options: options}->Delegation]}
  }
}

module Transfer = {
  open Protocol.Transfer

  let makeSimple = (
    ~data,
    ~fee=?,
    ~parameter=?,
    ~entrypoint=?,
    ~gasLimit=?,
    ~storageLimit=?,
    (),
  ) => {
    data: Simple(data),
    parameter: Options.makeParameter(~value=?parameter, ~entrypoint?, ()),
    options: Options.make(~fee?, ~gasLimit?, ~storageLimit?, ()),
  }

  let makeSimpleTez = (~parameter=?, ~entrypoint=?, ~destination, ~amount) =>
    makeSimple(
      ~parameter?,
      ~entrypoint?,
      ~data={destination: destination, amount: Amount.makeTez(amount)},
    )

  /* Tokens cannot define parameter and entrypoint, since they are
   already translated as parameters into an entrypoint */
  let makeSimpleToken = (~destination, ~amount, ~token, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) =>
    makeSimple(
      ~data={destination: destination, amount: Amount.makeToken(~amount, ~token)},
      ~fee?,
      ~gasLimit?,
      ~storageLimit?,
      (),
    )

  let makeBatch = (~source, ~transfers, ()) => {
    source: source,
    managers: transfers->Array.map(t => Transfer(t)),
  }

  let reduceArray = (transfers, f) =>
    transfers->Array.reduce(list{}, (acc, t: t) =>
      switch t.data {
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
    )

  let isNonNativeContractCall = (recipient, amount) =>
    recipient->PublicKeyHash.isContract && !(amount->ProtocolAmount.isToken)

  let hasParams = p => p.Options.value != None || p.entrypoint != None
}

module Multisig = {
  type call
  external fromTransfer: Protocol.Transfer.t => call = "%identity"
  external toTransfers: array<call> => array<Protocol.Transfer.t> = "%identity"

  @ocaml.doc(" Create a 0 tez transaction which call [entrypoint] of [destination] with [parameter] ")
  let makeCall = (~entrypoint, ~parameter, ~destination): call => {
    Transfer.makeSimpleTez(~parameter, ~entrypoint, ~destination, ~amount=Tez.zero, ())
    ->fromTransfer
  }

  @ocaml.doc(" Call 'propose' entrypoint of [destination] with [parameter] lambda ")
  let makeProposal = (~parameter, ~destination): call => {
    makeCall(~parameter, ~entrypoint="propose", ~destination)
  }

  @ocaml.doc(" Handle lambda and proposal creation to send [amount] tez from [sender] to [recipient] ")
  let makeTransfer = (~recipient: PublicKeyHash.t, ~amount: ProtocolAmount.t, ~sender) : call => {
    let amount = amount->ProtocolAmount.getTez->Option.getWithDefault(Tez.zero)->Tez.toBigNumber
    let lambda = PublicKeyHash.isImplicit(recipient)
      ? ReTaquito.Toolkit.Lambda.transferImplicit((recipient :> string), amount)
      : ReTaquito.Toolkit.Lambda.transferToContract((recipient :> string), amount)
    makeProposal(~parameter=lambda, ~destination=sender)
  }

  @ocaml.doc(" Wrap a list of calls into a batch to be able to use regular operation submission flow ")
  let wrap = (~source, transfers: array<call>) => {
    Transfer.makeBatch(~source, ~transfers=(transfers->toTransfers), ())
  }
}

module Origination = {
  let make = (
    ~balance=?,
    ~code,
    ~storage,
    ~delegate: option<PublicKeyHash.t>,
    ~gasLimit=?,
    ~storageLimit=?,
    ~fee=?,
    (),
  ) => Origination({
    balance: balance,
    delegate: delegate,
    code: code,
    storage: storage,
    options: {
      gasLimit: gasLimit,
      storageLimit: storageLimit,
      fee: fee,
    },
  })
}

let optionsSet = x =>
  switch x {
  | Delegation({options})
  | Origination({options})
  | Transfer({options}) =>
    ProtocolOptions.txOptionsSet(options)->Some
  }

let isContractCall = o =>
  switch o {
  | Origination(_) => false
  | Delegation(_) => false
  | Transfer(t: Protocol.Transfer.t) =>
    switch t.data {
    | Simple(data) =>
      data.destination->PublicKeyHash.isContract || data.amount->Protocol.Amount.getToken != None
    | FA2Batch(_) => true
    }
  }

module Simulation = {
  open Protocol.Simulation
  let sumFees = a =>
    a->Array.reduce(Tez.zero, (acc, sim) => {
      open Tez.Infix
      acc + sim.fee
    })

  let computeRevealFees = sim =>
    sim.revealSimulation->Option.mapWithDefault(Tez.zero, ({fee}) => fee)

  let getTotalFees = sim => {
    open Tez.Infix
    sim->computeRevealFees + sim.simulations->sumFees
  }
}
