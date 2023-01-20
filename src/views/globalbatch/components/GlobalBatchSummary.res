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

open ReactNative

@react.component
let make = (
  ~source: Account.t,
  ~ledgerState,
  ~dryRun,
  ~secondaryButton=?,
  ~operation: Protocol.batch,
  ~sendOperation,
  ~loading,
  ~setAdvancedOptions,
  ~advancedOptionsDisabled=true,
  ~onClose,
) => <>
  <ModalFormView
    title=I18n.Title.confirm_batch
    closing={
      open ModalFormView
      Close(_ => onClose())
    }>
    <View style=FormStyles.header>
      <Typography.Overline1> {I18n.Expl.global_batch->React.string} </Typography.Overline1>
    </View>
    {switch operation.managers {
    | [Delegation(_)] => React.null
    | _ =>
      <OperationSummaryView.Batch
        operation
        dryRun
        editAdvancedOptions={i => setAdvancedOptions(Some(i))}
        advancedOptionsDisabled
        hideBatchDetails=true
      />
    }}
    <SigningBlock
      accountKind=source.Account.kind
      state=ledgerState
      ?secondaryButton
      loading
      sendOperation={sendOperation(~operation)}
    />
  </ModalFormView>
</>
