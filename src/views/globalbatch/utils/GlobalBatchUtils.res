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
open GlobalBatchXfs
open GlobalBatchTypes

// Matchers
let matchFA2 = (m: Protocol.manager) =>
  switch m {
  | Transfer(t) =>
    switch t.data {
    | FA2Batch(b) => Some((b, t.options, t.parameter))
    | Simple(_) => None
    }
  | _ => None
  }

let matchTransfer = manager =>
  switch manager {
  | Transfer(t) => Some(t)
  | _ => None
  }

let replaceManagerInBatch = (batch: Protocol.batch, newManager, index) => {
  ...batch,
  managers: Array.replaceAtIndex(batch.managers, index, newManager),
}

let appendManagerToBatch = (~manager, ~batch: Protocol.batch) => {
  let newManager = manager
  let newManagers = Array.concat(batch.managers, [newManager])

  {source: batch.source, managers: newManagers}
}

let findTransferWithSameAddress = (managers: array<Protocol.manager>, searchedAddress) =>
  managers->Array.getIndexBy(manager =>
    switch manager {
    | Transfer(transferData) =>
      switch transferData.data {
      | Simple(_) => false
      | FA2Batch(batchData) => batchData.address == searchedAddress
      }
    | _ => false
    }
  )

let lookupByAddress = (managers, address) =>
  findTransferWithSameAddress(managers, address)->Option.flatMap(index =>
    managers
    ->Array.get(index)
    ->Option.flatMap(manager =>
      switch manager {
      | Transfer(t) => Some(t)
      | _ => None
      }
    )
    ->Option.map(transfer => (index, transfer))
  )

let addTransfer = (~transfer, ~batch) => {
  let newTransfer = transfer
  let nominalResult = () => appendManagerToBatch(~manager=Transfer(newTransfer), ~batch)

  switch newTransfer.data {
  | Simple(_) => nominalResult()
  | FA2Batch(newFa2Batch) =>
    batch.managers
    ->lookupByAddress(newFa2Batch.address)
    ->Option.map(((managerIndex, transfer)) =>
      switch transfer.data {
      | Simple(_) => nominalResult()
      | FA2Batch(existingFa2Batch) =>
        let transfers = List.concat(existingFa2Batch.transfers, newFa2Batch.transfers)

        let updatedFa2Batch = {...existingFa2Batch, transfers: transfers}

        let uptdatedManager = Transfer({...transfer, data: FA2Batch(updatedFa2Batch)})

        replaceManagerInBatch(batch, uptdatedManager, managerIndex)
      }
    )
    ->Option.getWithDefault(nominalResult())
  }
}

let rec removeTransfer = (~batch: Protocol.batch, ~coords: managerCoords) => {
  open Option
  let (i, j) = coords

  let result = switch j {
  | Some(j) =>
    batch.managers
    ->Array.get(i)
    ->flatMap(matchFA2)
    ->map(((fa2Batch, options, parameter)) => {
      let newTransfers = fa2Batch.transfers->List.keepWithIndex((_, i) => i != j)

      // If FA2Batch has no more transfers, we delete it
      if newTransfers == list{} {
        removeTransfer(~batch, ~coords=(i, None))
      } else {
        // Else update FA2Batch
        let newFa2Batch = {...fa2Batch, transfers: newTransfers}
        let newManager = Transfer({
          options: options,
          parameter: parameter,
          data: FA2Batch(newFa2Batch),
        })
        let newBatch = replaceManagerInBatch(batch, newManager, i)
        newBatch
      }
    })
    ->getWithDefault(batch)
  | None =>
    let newManagers = Array.removeAtIndex(batch.managers, i)
    {...batch, managers: newManagers}
  }
  result
}

let mergeTransfer = (
  newTr: Transfer.t,
  target: GlobalBatchTypes.managerCoords,
  batch: Protocol.batch,
) => {
  let (i, _) = target

  let (managerIndex, _) = target
  let managers = batch.managers

  managers
  ->Array.get(managerIndex)
  ->Option.flatMap(matchTransfer)
  ->Option.mapWithDefault(managers, oldTr =>
    switch (oldTr.data, newTr.data) {
    | (Simple(_), Simple(_)) =>
      let newManager = Transfer({...oldTr, data: newTr.data})
      let newBatch = replaceManagerInBatch(batch, newManager, i)
      newBatch.managers

    | (Simple(_), FA2Batch(_))
    | (FA2Batch(_), FA2Batch(_))
    | (FA2Batch(_), Simple(_)) =>
      let batchSpliced = removeTransfer(~batch, ~coords=target)
      let newBatch = addTransfer(~transfer=newTr, ~batch=batchSpliced)
      newBatch.managers
    }
  )
}

let set = (batch: Protocol.batch, coords, payload) => {
  let (amount, recipient, parameter) = payload
  let newTransfer =
    (Factories.makeTransferData(amount, recipient), parameter)->transferDataToTransfer

  let newManagers = mergeTransfer(newTransfer, coords, batch)

  {...batch, managers: newManagers}
}

let add = (~payload, ~batch) => addTransfer(~transfer=transferPayloadToTransfer(payload), ~batch)

let remove = removeTransfer

// Batch genesis with first transfer
let makeInitialBatch = (~payload: transferPayload, ~account: Account.t) => {
  let transfer = transferPayloadToTransfer(payload)

  ProtocolHelper.Transfer.makeBatch(~source=account, ~transfers=[transfer], ())
}

let addToExistingOrNew = (account: Account.t, batch, payload: transferPayload) =>
  switch batch {
  | Some(batch) => add(~payload, ~batch)
  | None => makeInitialBatch(~payload, ~account)
  }
