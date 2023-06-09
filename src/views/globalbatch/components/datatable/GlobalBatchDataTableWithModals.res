/* ************************************************************************** */
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

open ReactNative

let getRowAndCoords = (rows, i) =>
  switch i {
  | None => None
  | Some(i) =>
    switch rows[i] {
    | Some(vs) => Some(vs)
    | None => None
    }
  }

let useConfirmDeleteAllModal = (~onPressConfirmDelete) =>
  ModalDialogConfirm.useModal(~action=() => {
    onPressConfirmDelete()
    Promise.ok()
  }, ~title=I18n.Title.delete_batch, ~cancelText=I18n.Btn.cancel, ~actionText=I18n.Btn.delete, ~contentText=I18n.Expl.delete_batch)

module Details = {
  let detailsStyles = {
    open Style
    StyleSheet.create({
      "container": style(~minHeight=0.->dp, ()),
      "label": style(~marginBottom=4.->dp, ~marginTop=24.->dp, ()),
    })
  }

  open ProtocolOptions.TransactionParameters

  @react.component
  let make = (~closeAction, ~validState as (amount, _, p: ProtocolOptions.parameter)) => {
    let amount = amount->ProtocolAmount.getTez

    <ModalFormView closing=ModalFormView.Close(closeAction) title=I18n.Title.details>
      <FormLabel style={detailsStyles["label"]} label=I18n.Label.entrypoint />
      <CodeView
        style={detailsStyles["container"]}
        text={p.entrypoint->ProtocolOptions.TransactionParameters.getEntrypoint}
      />
      <FormLabel style={detailsStyles["label"]} label=I18n.Label.parameters />
      <CodeView
        style={detailsStyles["container"]}
        text={
          let p = p.value->ProtocolOptions.TransactionParameters.getValue
          p->MichelineMichelsonV1Expression.toString->Option.default("")
        }
      />
      {amount->ReactUtils.mapOpt(a => <>
        <FormLabel style={detailsStyles["label"]} label=I18n.Label.send_amount />
        <CodeView style={detailsStyles["container"]} text={a->Tez.toString->I18n.tez_amount} />
      </>)}
    </ModalFormView>
  }
}

let makeValidState = (
  amount: Protocol.Amount.t,
  sender: Alias.t,
  recipient: PublicKeyHash.t,
  parameter,
): SendForm.validState => {
  amount: amount,
  sender: sender,
  recipient: Address(recipient),
  parameter: parameter.ProtocolOptions.value,
  entrypoint: parameter.ProtocolOptions.entrypoint,
}

module AdvancedEditView = {
  @react.component
  let make = (
    ~signer,
    ~operations: array<Umami.Protocol.manager>,
    ~dryRun,
    ~index,
    ~onSubmit,
    ~closeAction,
  ) => {
    <ModalFormView title=I18n.Label.advanced_options closing=ModalFormView.Close(closeAction)>
      <AdvancedOptionsView signer operations dryRun index onSubmit token=None />
    </ModalFormView>
  }
}

module EditView = {
  @react.component
  let make = (~amount, ~recipient, ~account: Alias.t, ~parameter, ~onSubmit, ~closeAction) => {
    let dummyIndex = 3
    <SendView.EditionView
      account
      initValues={makeValidState(amount, account, recipient, parameter)}
      onSubmit
      index=dummyIndex
      closeAction
      loading=false //FIXME?
    />
  }
}
let indexToCoords = (i, vs) => vs->Array.get(i)->Option.map(((coords, _)) => coords)

@react.component
let make = (
  ~account: Alias.t,
  ~batch: array<Umami.Protocol.manager>,
  ~replaceBatchItem: (Umami.GlobalBatchTypes.rowData, unit => unit) => unit,
  ~removeBatchItem,
  ~dryRun: option<Umami.Protocol.Simulation.results>,
  ~setBatchAndSim,
  ~onDeleteAll,
) => {
  let (openConfirmModal, _, confirmDeleteModal) = useConfirmDeleteAllModal(
    ~onPressConfirmDelete=onDeleteAll,
    (),
  )

  let (openAction, closeAction, wrapModal) = ModalAction.useModal()

  let (indexDetails, setIndexDetails) = React.useState(() => None)

  let (indexToEdit, setIndexToEdit) = React.useState(() => None)
  let (advancedEditMode, setAdvancedEditMode) = React.useState(() => false)

  let indexedRows = GlobalBatchXfs.batchToIndexedRows(batch)

  let (openDetailsModal, detailsModal) = {
    let (openDetailsModal, closeAction, modal) = ModalAction.useModal()

    let details =
      indexDetails
      ->Option.flatMap(i => indexedRows->Array.get(i))
      ->Option.map(snd)
      ->Option.flatMap(((_, recipient, _) as vs) =>
        recipient->PublicKeyHash.isContract ? Some(vs) : None
      )
      ->ReactUtils.mapOpt(validState => modal(<Details closeAction validState />))

    (openDetailsModal, details)
  }

  let rowToEdit = getRowAndCoords(indexedRows, indexToEdit)
  let trRowToEdit = rowToEdit->Option.flatMap(((coords, p)) => {
    let (amount, recipient, parameter) = p
    Some((coords, amount, recipient, parameter))
  })

  let resetEdit = () => {
    setAdvancedEditMode(_ => false)
    setIndexToEdit(_ => None)
    closeAction()
  }

  let handleAdvancedSubmit = (batch, dryRun) => {
    setBatchAndSim((account.address, batch, Some(dryRun)))
    resetEdit()
  }

  let handleNominalSubmit = (coords, vs: SendForm.validState) => {
    let transferPayload = (
      vs.amount,
      vs.recipient->FormUtils.Alias.address,
      {
        open ProtocolOptions
        {entrypoint: vs.entrypoint, value: vs.parameter}
      },
    )
    replaceBatchItem((coords, transferPayload), resetEdit)
  }

  let handleRemove = i =>
    indexToCoords(i, indexedRows)->Option.map(coords => removeBatchItem(coords))->ignore

  <>
    <GlobalBatchDataTable
      indexedRows
      onDeleteAll=openConfirmModal
      dryRun
      onDelete=handleRemove
      onDetails={i => {
        setIndexDetails(_ => Some(i))
        openDetailsModal()
      }}
      onEdit={i => {
        setIndexToEdit(_ => Some(i))
        openAction()
      }}
      onAdvanced={i => {
        setAdvancedEditMode(_ => true)
        setIndexToEdit(_ => Some(i))
        openAction()
      }}
    />
    {confirmDeleteModal()}
    detailsModal
    {trRowToEdit
    ->Option.mapWithDefault(React.null, row => {
      let (coords, amount, recipient, parameter) = row
      let closeAction = resetEdit

      {
        switch (dryRun, advancedEditMode ? Alias.toAccount(account)->ResultEx.toOption : None) {
        | (Some(dryRun), Some(signer)) =>
          let operations = batch
          let (index, _) = coords
          let onSubmit = handleAdvancedSubmit
          <AdvancedEditView signer operations dryRun index onSubmit closeAction />
        | _ =>
          <EditView
            amount
            parameter
            recipient
            account
            onSubmit={vs => handleNominalSubmit(coords, vs)}
            closeAction
          />
        }
      }
    })
    ->wrapModal}
  </>
}
