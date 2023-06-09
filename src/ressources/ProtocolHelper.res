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
  let makeSimpleToken = (
    ~source,
    ~destination,
    ~amount,
    ~token,
    ~fee=?,
    ~gasLimit=?,
    ~storageLimit=?,
    (),
  ) =>
    makeSimple(
      ~data={destination: destination, amount: Amount.makeToken(~amount, ~token, ~source)},
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
  open Michelson.MichelsonV1Expression
  open Michelson.MichelsonV1Expression.Constructors
  open Michelson.MichelsonV1Expression.Instructions
  open Michelson.MichelsonV1Expression.Types
  type call = Protocol.Transfer.t
  external jsonToMichelson0: Js.Json.t => ReTaquitoTypes.MichelsonV1Expression.t = "%identity"
  external michelsonToJson0: ReTaquitoTypes.MichelsonV1Expression.t => Js.Json.t = "%identity"
  let jsonToMichelson = array => {
    array->Js.Json.array->jsonToMichelson0
  }
  let michelsonToJson = lambda => {
    lambda->michelsonToJson0->Js.Json.decodeArray->Option.getExn
  }

  let emptyLambda: ReTaquitoTypes.MichelsonV1Expression.t = {
    [_DROP, _NIL(operation)]->seq->jsonToMichelson0
  }

  @ocaml.doc(" Concatenate two lists of operations ")
  let appendLambda = (
    hd: ReTaquitoTypes.MichelsonV1Expression.t,
    tl: ReTaquitoTypes.MichelsonV1Expression.t,
  ): ReTaquitoTypes.MichelsonV1Expression.t => {
    let hd = michelsonToJson(hd)
    let tl = michelsonToJson(tl)
    Array.concat(hd, tl->Array.sliceToEnd(2))->jsonToMichelson
  }

  let lambdaContractCall = (
    ~amount=ReBigNumber.zero,
    ~entrypoint,
    ~arg_type,
    ~arg_value,
    contract: PublicKeyHash.t,
  ) => {
    [
      _DROP,
      _NIL(operation),
      _PUSH(address, string((contract :> string) ++ "%" ++ entrypoint)),
      _CONTRACT(arg_type),
      [_IF_NONE([[_UNIT, _FAILWITH]->seq]->seq, []->seq)]->seq,
      _PUSH(mutez, int(amount->ReBigNumber.toString)),
      _PUSH(arg_type, arg_value),
      _TRANSFER_TOKENS,
      _CONS,
    ]
    ->seq
    ->jsonToMichelson0
  }

  module FA = {
    module Parameters = {
      module Type = {
        type t = Js.Json.t
        let fa12Transfer: Js.Json.t = pair(address, pair(address, nat))
        let fa2Transfer: Js.Json.t = list(pair(address, list(pair(address, pair(nat, nat)))))
      }

      module Value = {
        type t = Js.Json.t
        let fa12Transfer = (
          from: PublicKeyHash.t,
          to: PublicKeyHash.t,
          amount: ReTaquitoTypes.BigNumber.fixed,
        ) => _Pair(string((from :> string)), _Pair(string((to :> string)), int((amount :> string))))

        let fa2Transfer = (parameters: array<ReTaquitoTypes.FA2.transferParam>) =>
          Array.map(parameters, x =>
            _Pair(
              string((x.from_ :> string)),
              Array.map(x.ReTaquitoTypes.FA2.txs, x =>
                _Pair(
                  string((x.to_ :> string)),
                  _Pair(int((x.token_id :> string)), int((x.amount :> string))),
                )
              )->seq,
            )
          )->seq
      }

      type t = {
        type_: Type.t,
        value: Value.t,
      }

      let fa12Transfer = (
        ~from_: PublicKeyHash.t,
        ~to_: PublicKeyHash.t,
        ~amount: ReTaquitoTypes.BigNumber.fixed,
      ): t => {
        type_: Type.fa12Transfer,
        value: Value.fa12Transfer(from_, to_, amount),
      }

      let fa2Transfer: array<ReTaquitoTypes.FA2.transferParam> => t = parameters => {
        type_: Type.fa2Transfer,
        value: Value.fa2Transfer(parameters),
      }
    }

    let makeCall = (
      recipient: PublicKeyHash.t,
      amount: ReBigNumber.t,
      entrypoint: string,
      parameters: Parameters.t,
    ): ReTaquitoTypes.Lambda.t =>
      lambdaContractCall(
        recipient,
        ~amount,
        ~entrypoint,
        ~arg_type=parameters.type_,
        ~arg_value=parameters.value,
      )
  }

  @ocaml.doc(" Create lambda from a list of FA2 transfers ")
  let batchFA2 = (batch: Protocol.Transfer.batchFA2) => {
    open ReTaquitoTypes.BigNumber
    let transfers = List.reduce(batch.transfers, PublicKeyHash.Map.empty, (acc, transfer) => {
      let pkh = transfer.content.amount.source
      let transfers = PublicKeyHash.Map.getWithDefault(acc, pkh, list{})
      let transfer = {
        ReTaquitoTypes.FA2.to_: transfer.content.destination,
        token_id: transfer.tokenId->Int64.of_int->fromInt64->toFixed,
        amount: transfer.content.amount.amount->TokenRepr.Unit.toBigNumber->toFixed,
      }
      PublicKeyHash.Map.set(acc, pkh, list{transfer, ...transfers})
    })
    let parameter =
      PublicKeyHash.Map.toArray(transfers)
      ->Array.map(((k, v)): ReTaquitoTypes.FA2.transferParam => {
        from_: k,
        txs: List.toArray(v),
      })
      ->FA.Parameters.fa2Transfer
    FA.makeCall(batch.address, ReBigNumber.zero, "transfer", parameter)
  }

  @ocaml.doc(" Create lambda to send [amount] tez or token from [sender] to [recipient] ")
  let transfer = (recipient: PublicKeyHash.t, amount: ProtocolAmount.t) =>
    switch amount {
    | Tez(amount) =>
      let amount = amount->Tez.toBigNumber
      PublicKeyHash.isImplicit(recipient)
        ? ReTaquito.Toolkit.Lambda.transferImplicit((recipient :> string), amount)
        : ReTaquito.Toolkit.Lambda.transferToContract((recipient :> string), amount)
    | Token({amount, token, source}) =>
      open ReTaquitoTypes.BigNumber
      let amount = amount->TokenRepr.Unit.toBigNumber->toFixed
      let parameter = switch token.kind {
      | FA1_2 => FA.Parameters.fa12Transfer(~from_=source, ~to_=recipient, ~amount)
      | FA2(tokenID) =>
        FA.Parameters.fa2Transfer([
          {
            from_: source,
            txs: [
              {to_: recipient, token_id: tokenID->Int64.of_int->fromInt64->toFixed, amount: amount},
            ],
          },
        ])
      }
      FA.makeCall(token.address, ReBigNumber.zero, "transfer", parameter)
    }

  @ocaml.doc(
    " Create a 0 tez transaction which call [entrypoint] of [destination] with [parameter] "
  )
  let call = (~entrypoint, ~parameter, destination) => {
    Protocol.Transfer(
      Transfer.makeSimpleTez(~parameter, ~entrypoint, ~destination, ~amount=Tez.zero, ()),
    )
  }

  let lambdaPropose = (~contract, ~value) => {
    let arg_type = lambda(unit, list(operation))
    let arg_value = value->michelsonToJson0
    lambdaContractCall(contract, ~entrypoint="propose", ~arg_type, ~arg_value)
  }

  let lambdaApproveOrExecute = (~contract, ~entrypoint, ~value) => {
    let arg_type = nat
    let arg_value = value->michelsonToJson0
    lambdaContractCall(contract, ~entrypoint, ~arg_type, ~arg_value)
  }

  @ocaml.doc(" Convert a Protocol.manager into a lambda ")
  let fromManager = (manager: Protocol.manager): ReTaquito.Toolkit.Lambda.t =>
    switch manager {
    | Delegation({delegate: Delegate(baker)}) =>
      ReTaquito.Toolkit.Lambda.setDelegate((baker :> string))
    | Delegation({delegate: Undelegate(_)}) => ReTaquito.Toolkit.Lambda.removeDelegate()
    | Transfer({
        data: Simple({destination}),
        parameter: {entrypoint: Some("propose"), value: Some(value)},
      }) =>
      lambdaPropose(~contract=destination, ~value)
    | Transfer({
        data: Simple({destination}),
        parameter: {entrypoint: Some(("approve" | "execute") as entrypoint), value: Some(value)},
      }) =>
      lambdaApproveOrExecute(~contract=destination, ~entrypoint, ~value)
    | Transfer({data: Simple({destination, amount}), parameter: {entrypoint: None, value: None}}) =>
      transfer(destination, amount)
    | Transfer({data: FA2Batch(batch)}) => batchFA2(batch)
    | _ => Js.Exn.raiseError(__LOC__ ++ ": unsupported")
    }

  @ocaml.doc(" Create lambda from a list of operations ")
  let batch = (array: array<Protocol.manager>): ReTaquitoTypes.MichelsonV1Expression.t =>
    Array.reduce(array, emptyLambda, (acc, manager) => {
      let lambda = fromManager(manager)
      appendLambda(acc, lambda)
    })
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
