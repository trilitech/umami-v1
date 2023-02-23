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
type batchAndSim = (PublicKeyHash.t, array<Protocol.manager>, option<Protocol.Simulation.results>)

type state = {
  isSimulating: bool,
  addTransfer: (transferPayload, PublicKeyHash.t, unit => unit) => unit,
  addTransfers: array<transferPayload> => unit,
  dryRun: option<Protocol.Simulation.results>,
  setBatchAndSim: batchAndSim => unit,
  batch: option<array<Protocol.manager>>,
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
  all->Array.getBy(((pkh, _, _)) => pkh == account)

let updatedBatchAndSims = ((pkh, batch, sim): batchAndSim, allBatches: array<batchAndSim>) => {
  let accountToAdd = pkh

  let accountHasBatch = allBatches->Array.some(((pkh, _, _)) => pkh == accountToAdd)

  if accountHasBatch {
    allBatches->Array.map(((pkh, b, s)) => pkh == accountToAdd ? (pkh, batch, sim) : (pkh, b, s))
  } else {
    allBatches->Array.concat([(pkh, batch, sim)])
  }
}

let removeBatchAndSim = (account: PublicKeyHash.t, allBatches: array<batchAndSim>) =>
  allBatches->Array.keep(((pkh, _, _)) => pkh != account)
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
  let make = (~children, ~selectedAccount: PublicKeyHash.t) => {
    let addToast = LogsContext.useToast()
    let (operationSimulate, sendOperationSimulate) = StoreContext.Operations.useSimulate()
    let isSimulating = operationSimulate->ApiRequest.isLoading

    let (allBatchAndSim, setAllBatchAndSim) = React.useState(() => [])

    let batchAndSim = getBatch(selectedAccount, allBatchAndSim)
    let (batch, dryRun) = switch batchAndSim {
    | Some(_pkh, batch, dryRun) => (Some(batch), dryRun)
    | None => (None, None)
    }

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

    let simulateThenSet = (managers, onSucces) => {
      let update = dryRun => {
        let updatedBatches = updatedBatchAndSims(
          (selectedAccount, managers, dryRun),
          allBatchAndSim,
        )
        setAllBatchAndSim(_ => updatedBatches)
        onSucces()
      }
      PublicKeyHash.isImplicit(selectedAccount)
        ? {Account.address: selectedAccount, name: "", kind: Encrypted} // FIXME: kind?
          ->sendOperationSimulate(managers)
          ->Promise.getOk(dryRun => update(Some(dryRun)))
        : update(None)
    }

    // We can define account from outside when adding
    let addTransfer = (payload: transferPayload, account: PublicKeyHash.t, onDone) => {
      let existingBatchAndSim = getBatch(account, allBatchAndSim)

      let newBatch = switch existingBatchAndSim {
      | Some((_, b, _)) => GlobalBatchUtils.addToExistingOrNew(Some(b), payload)
      | None => GlobalBatchUtils.addToExistingOrNew(None, payload)
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

        if newBatch->Array.length > 0 {
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
      | None => ()
      }

    let addTransfers = (trs: array<transferPayload>) =>
      trs
      ->Array.reduce(batch, (acc, curr) => GlobalBatchUtils.addToExistingOrNew(acc, curr)->Some)
      ->Option.map(acc => simulateThenSet(acc, () => ()))
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
  | Some(account) => <ProviderPrivate selectedAccount=account.address> children </ProviderPrivate>
  | None => children
  }
}
let useGlobalBatchContext = () => React.useContext(context)
