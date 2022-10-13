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
open Protocol.Transfer
open SendForm

open GlobalBatchTypes

/*
 * Transformers for global batch related types go here
 */

module Factories = {
  let makeTransferFA2 = (token: ProtocolAmount.token, destination: PublicKeyHash.t) => {
    let newTransferFA2: Protocol.Transfer.transferFA2 = {
      tokenId: token.token.kind->TokenRepr.kindId,
      content: {
        amount: token,
        destination: destination,
      },
    }
    newTransferFA2
  }

  let makeFA2Data = (token, recipient) => {
    let transferFa2 = makeTransferFA2(token, recipient)
    let batchFA2: Protocol.Transfer.batchFA2 = {
      address: token.token.address,
      transfers: list{transferFa2},
    }
    FA2Batch(batchFA2)
  }

  let makeTransferData = (amount, recipient) => {
    let simple = Simple({amount: amount, destination: recipient})

    switch amount {
    | Tez(_) => simple
    | Token(token) =>
      if TokenRepr.isFa2(token.token) {
        makeFA2Data(token, recipient)
      } else {
        simple
      }
    }
  }
}

let validStateToTransferPayload = (validState: SendForm.validState) => {
  let p: transferPayload = (
    validState.amount,
    validState.recipient->FormUtils.Alias.address,
    ProtocolOptions.makeParameter(
      ~value=?validState.parameter,
      ~entrypoint=?validState.entrypoint,
      (),
    ),
  )
  p
}

let transferPayloadToTransferData = ((amount, destination, parameter): transferPayload) =>
  switch amount {
  | Tez(_) => (Simple({amount: amount, destination: destination}), parameter)
  | Token(token) =>
    if TokenRepr.isFa2(token.token) {
      (Factories.makeFA2Data(token, destination), parameter)
    } else {
      (Simple({amount: amount, destination: destination}), parameter)
    }
  }

// Adds default options to Trasfer.data to create a Transfer.t
let transferDataToTransfer = ((data: Transfer.data, parameter)) => {
  let options = ProtocolOptions.make()
  let result: Transfer.t = {data: data, options: options, parameter: parameter}
  result
}
let transferPayloadToTransfer = (p: transferPayload) => {
  let result = transferPayloadToTransferData(p)->transferDataToTransfer
  result
}

let batchToIndexedRows = (batch: Protocol.batch) => {
  open Protocol.Amount
  let result = []

  batch.managers
  ->Array.mapWithIndex((i, manager) => {
    switch manager {
    | Delegation(_) => ()
    | Origination(_) => ()
    | Transfer(transfer) =>
      switch transfer.data {
      | FA2Batch(fa2Batch) =>
        fa2Batch.transfers
        ->List.mapWithIndex((j, {content}) => {
          let payload = (Token(content.amount), content.destination, transfer.parameter)
          result->Js.Array2.push(((i, Some(j)), payload))->ignore
        })
        ->ignore
      | Simple(simple) =>
        let payload = (simple.amount, simple.destination, transfer.parameter)
        result->Js.Array2.push(((i, None), payload))->ignore
      }
    }
    manager
  })
  ->ignore

  result
}
