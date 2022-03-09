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

let getRowAndCoords = (rows, i) => {
  switch (i) {
  | None => None
  | Some(i) =>
    switch (rows[i]) {
    | Some(vs) => Some(vs)
    | None => None
    }
  };
};

let useConfirmDeleteAllModal = (~onPressConfirmDelete) =>
  ModalDialogConfirm.useModal(
    ~action=
      () => {
        onPressConfirmDelete();
        Promise.ok();
      },
    ~title=I18n.Title.delete_batch,
    ~cancelText=I18n.Btn.cancel,
    ~actionText=I18n.Btn.delete,
    ~contentText=I18n.Expl.delete_batch,
  );

let makeValidState =
    (amount: Protocol.Amount.t, sender: Account.t, recipient: PublicKeyHash.t)
    : SendForm.validState => {
  amount,
  sender,
  recipient: Address(recipient),
};

module EditView = {
  [@react.component]
  let make =
      (
        ~advancedEditMode,
        ~managerIndex,
        ~amount,
        ~recipient,
        ~account,
        ~operation,
        ~dryRun,
        ~onAdvancedSubmit,
        ~onNominalEdit,
        ~onClose,
      ) => {
    let dummyIndex = 3;
    advancedEditMode
      ? {
        ModalFormView.(
          operation->Option.mapWithDefault(React.null, operation =>
            <ModalFormView
              title=I18n.Label.advanced_options closing={Close(onClose)}>
              <AdvancedOptionsView
                operation
                dryRun
                index=managerIndex
                onSubmit={(batch, dryRun) => {
                  onAdvancedSubmit(batch, dryRun)
                }}
                token=None
              />
            </ModalFormView>
          )
        );
      }
      : <SendView
          account
          closeAction=onClose
          onEdit=onNominalEdit
          initalStep=SendView.(
            EditStep(dummyIndex, makeValidState(amount, account, recipient))
          )
        />;
  };
};
let indexToCoords = (i, vs) => {
  vs->Array.get(i)->Option.map(((coords, _)) => coords);
};

[@react.component]
let make =
    (
      ~batch,
      ~simulations: array(Umami.Protocol.Simulation.resultElt),
      ~replaceBatchItem,
      ~removeBatchItem,
      ~dryRun,
      ~setBatchAndSim,
      ~onDeleteAll,
    ) => {
  let (openConfirmModal, _, confirmDeleteModal) =
    useConfirmDeleteAllModal(~onPressConfirmDelete=onDeleteAll, ());

  let (openModal, closeModal, inModal) = ModalAction.useModal();

  let (indexToEdit, setIndexToEdit) = React.useState(() => None);
  let (advancedEditMode, setAdvancedEditMode) = React.useState(() => false);

  let indexedRows = GlobalBatchXfs.batchToIndexedRows(batch);

  let rowToEdit = getRowAndCoords(indexedRows, indexToEdit);
  let trRowToEdit =
    rowToEdit->Option.flatMap(((coords, p)) => {
      let (amount, recipient) = p;
      Some((coords, amount, recipient));
    });

  let resetEdit = () => {
    setAdvancedEditMode(_ => false);
    setIndexToEdit(_ => None);
    closeModal();
  };

  let handleAdvancedSubmit = (batch, dryRun) => {
    setBatchAndSim((batch, dryRun));
    resetEdit();
  };

  let handleNominalSubmit = (coords, vs: SendForm.validState) => {
    let transferPayload = (vs.amount, vs.recipient->FormUtils.Alias.address);
    replaceBatchItem((coords, transferPayload), resetEdit);
  };

  let handleRemove = i => {
    indexToCoords(i, indexedRows)
    ->Option.map(coords => {
        removeBatchItem(coords);
        ();
      })
    ->ignore;
  };

  <>
    <GlobalBatchDataTable
      indexedRows
      onDeleteAll=openConfirmModal
      simulations
      onDelete=handleRemove
      onEdit={i => {
        setIndexToEdit(_ => Some(i));
        openModal();
        ();
      }}
      onAdvanced={i => {
        setAdvancedEditMode(_ => true);
        setIndexToEdit(_ => Some(i));
        openModal();
        ();
      }}
    />
    {confirmDeleteModal()}
    {trRowToEdit
     ->Option.mapWithDefault(
         React.null,
         row => {
           let (coords, amount, recipient) = row;

           let (managerIndex, _) = coords;
           <EditView
             advancedEditMode
             managerIndex
             amount
             recipient
             account={batch.source}
             operation={Some(batch)}
             dryRun
             onAdvancedSubmit=handleAdvancedSubmit
             onNominalEdit={vs => {handleNominalSubmit(coords, vs)}}
             onClose=resetEdit
           />;
         },
       )
     ->inModal}
  </>;
};
