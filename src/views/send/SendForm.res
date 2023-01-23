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

open Protocol

module StateLenses = %lenses(
  type state = {
    amount: string,
    sender: Alias.t,
    recipient: FormUtils.Alias.any,
  }
)

type validState = {
  amount: Protocol.Amount.t,
  sender: Alias.t,
  recipient: FormUtils.Alias.t,
  entrypoint: option<ProtocolOptions.TransactionParameters.entrypoint>,
  parameter: option<ProtocolOptions.TransactionParameters.value>,
}

let unsafeExtractValidState = (token, state: StateLenses.state): validState => {
  amount: state.amount->FormUtils.parseAmount(token)->FormUtils.Unsafe.getAmount,
  sender: state.sender,
  recipient: state.recipient->FormUtils.Unsafe.account,
  entrypoint: None,
  parameter: None,
}

let toState = (vs: validState): StateLenses.state => {
  amount: vs.amount->Protocol.Amount.toString,
  sender: vs.sender,
  recipient: vs.recipient->FormUtils.Alias.Valid,
}

include ReForm.Make(StateLenses)

let buildTransfer = (inputTransfers, source) => {
  let transfers =
    inputTransfers
    ->List.map((t: validState) => {
      let destination = t.recipient->FormUtils.Alias.address
      let data = {
        open Transfer
        {destination: destination, amount: t.amount}
      }
      ProtocolHelper.Transfer.makeSimple(~data, ())
    })
    ->List.toArray

  ProtocolHelper.Transfer.makeBatch(~source, ~transfers, ())
}

let buildTransaction = (batch: list<validState>, getImplicitFromAlias) =>
  switch batch {
  | list{} => assert false
  | list{first, ..._} as inputTransfers =>
    let source = getImplicitFromAlias(first.sender)
    buildTransfer(inputTransfers, source)
  }
