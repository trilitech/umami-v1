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

open GlobalBatchTypes
type batchAndSim = (Protocol.batch, Umami.Protocol.Simulation.results)

type state = {
  isSimulating: bool,
  addTransfer: (transferPayload, Account.t, unit => unit) => unit,
  addTransfers: array<transferPayload> => unit,
  dryRun: option<Umami.Protocol.Simulation.results>,
  setBatchAndSim: batchAndSim => unit,
  batch: option<Protocol.batch>,
  resetGlobalBatch: unit => unit,
  removeBatchItem: GlobalBatchTypes.managerCoords => unit,
  replaceBatchItem: (GlobalBatchTypes.rowData, unit => unit) => unit,
}

let initialState = {
  isSimulating: false,
  addTransfer: (_, _, _) => (),
  addTransfers: _ => (),
  dryRun: None,
  setBatchAndSim: _ => (),
  batch: None,
  resetGlobalBatch: () => (),
  removeBatchItem: _ => (),
  replaceBatchItem: (_, _) => (),
}

let context = React.createContext(initialState)

let getBatch = (account, all: array<batchAndSim>) =>
  all->Array.getBy(((b, _)) => b.source == account)

let updatedBatchAndSims = ((batch, sim): batchAndSim, allBatches: array<batchAndSim>) => {
  let accountToAdd = batch.source

  let accountHasBatch = allBatches->Array.some(((b, _)) => b.source == accountToAdd)

  if accountHasBatch {
    allBatches->Array.map(((b, s)) => b.source == accountToAdd ? (batch, sim) : (b, s))
  } else {
    allBatches->Array.concat([(batch, sim)])
  }
}

let removeBatchAndSim = (account: Account.t, allBatches: array<batchAndSim>) =>
  allBatches->Array.keep(((b, _)) => b.source != account)
module Provider = {
  let makeProps = (~value, ~children, ()) =>
    {
      "value": value,
      "children": children,
    }

  let make = React.Context.provider(context)
}

module ProviderPrivate = {
  @react.component
  let make = (~children, ~selectedAccount: Account.t) => {
    let addToast = LogsContext.useToast()
    let (operationSimulate, sendOperationSimulate) = StoreContext.Operations.useSimulate()
    let isSimulating = operationSimulate->ApiRequest.isLoading

    let (allBatchAndSim, setAllBatchAndSim) = React.useState(() => [])

    let batchAndSim = getBatch(selectedAccount, allBatchAndSim)
    let batch = batchAndSim->Option.map(((b, _)) => b)
    let dryRun = batchAndSim->Option.map(((_, s)) => s)

    // Reset globalBatch for selected account
    // by removing his batchAndSim from allBatchAndSims
    let resetGlobalBatch = () =>
      switch batchAndSim {
      | Some(_) =>
        setAllBatchAndSim(allBatchAndSim => removeBatchAndSim(selectedAccount, allBatchAndSim))
      | None => ()
      }

    let setBatchAndSim = bs => {
      let updatedBatches = updatedBatchAndSims(bs, allBatchAndSim)
      setAllBatchAndSim(_ => updatedBatches)
      ()
    }

    let simulateThenSet = (b, onSucces) => {
      sendOperationSimulate(b)->Promise.getOk(dryRun => {
        let updatedBatches = updatedBatchAndSims((b, dryRun), allBatchAndSim)

        setAllBatchAndSim(_ => updatedBatches)
        onSucces()
      })
      ()
    }

    // We can define account from outside when adding
    let addTransfer = (payload: transferPayload, account: Account.t, onDone) => {
      let existingBatchAndSim = getBatch(account, allBatchAndSim)

      let newBatch = switch existingBatchAndSim {
      | Some((b, _)) => GlobalBatchUtils.addToExistingOrNew(account, Some(b), payload)
      | None => GlobalBatchUtils.addToExistingOrNew(account, None, payload)
      }

      simulateThenSet(newBatch, () => {
        addToast(Logs.info(~origin=Batch, "Transaction inserted into batch"))
        onDone()
      })
    }

    let removeBatchItem = coords =>
      switch batch {
      | Some(batch) =>
        let newBatch = GlobalBatchUtils.remove(~batch, ~coords)

        if newBatch.managers->Array.length > 0 {
          simulateThenSet(newBatch, () => ())
        } else {
          resetGlobalBatch()
        }
      | None => ()
      }

    let replaceBatchItem = (rowData: GlobalBatchTypes.rowData, onSuccess) =>
      switch batch {
      | Some(batch) =>
        let (coords, payload) = rowData
        let newBatch = GlobalBatchUtils.set(batch, coords, payload)
        simulateThenSet(newBatch, onSuccess)
        ()
      | None => ()
      }

    let addTransfers = (trs: array<transferPayload>) =>
      Some(selectedAccount)
      ->Option.flatMap(sender =>
        trs->Array.reduce(batch, (acc, curr) =>
          GlobalBatchUtils.addToExistingOrNew(sender, acc, curr)->Some
        )
      )
      ->Option.map(b => simulateThenSet(b, () => ()))
      ->ignore

    <Provider
      value={
        isSimulating: isSimulating,
        addTransfer: addTransfer,
        addTransfers: addTransfers,
        dryRun: dryRun,
        resetGlobalBatch: resetGlobalBatch,
        removeBatchItem: removeBatchItem,
        replaceBatchItem: replaceBatchItem,
        setBatchAndSim: setBatchAndSim,
        batch: batch,
      }>
      children
    </Provider>
  }
}

@react.component
let make = (~children) => {
  let selectedAccount = StoreContext.SelectedAccount.useGetAtInit()
  switch selectedAccount {
  | Some(account) => <ProviderPrivate selectedAccount=account> children </ProviderPrivate>
  | None => children
  }
}
let useGlobalBatchContext = () => React.useContext(context)
