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

let useSendTransfer = () => {
  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate()

  let sendTransfer = (~source, ~operations, signingIntent) => {
    let operation = {Protocol.source: source, managers: operations}
    sendOperation({operation: operation, signingIntent: signingIntent})
  }
  (operationRequest, sendTransfer)
}

@react.component
let make = (
  ~source,
  ~operations:option<array<Protocol.manager>>,
  ~dryRun,
  ~closeModal,
  ~resetGlobalBatch,
  ~ledgerState,
) => {
  let (sendTransferState, sendTransfer) = useSendTransfer()

  let loading = sendTransferState->ApiRequest.isLoading

  let handlePressCancel = _ => {
    closeModal()
    open Routes
    push(Operations)
  }

  let renderSummary = () =>
    operations->Option.mapWithDefault(React.null, operations =>
      dryRun->Option.mapWithDefault(React.null, dryRun =>
        <GlobalBatchSummary
          source
          loading
          ledgerState
          dryRun
          operations=operations
          sendOperation=sendTransfer(~source)
          setAdvancedOptions={_ => ()}
          advancedOptionsDisabled=true
          onClose=closeModal
        />
      )
    )

  switch sendTransferState {
  | Done(Ok({hash}), _) =>
    resetGlobalBatch()

    <ModalFormView
      closing={
        open ModalFormView
        Close(_ => closeModal())
      }>
      <SubmittedView hash onPressCancel=handlePressCancel submitText=I18n.Btn.go_operations />
    </ModalFormView>
  | _ => renderSummary()
  }
}
