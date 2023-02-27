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

module SignStep = {
  @react.component
  let make = (~source, ~dryRun, ~operations, ~resetGlobalBatch, ~closeAction) => {
    let ledgerState = React.useState(() => None)
    <SignGlobalBatch dryRun source operations resetGlobalBatch closeAction ledgerState />
  }
}

type step =
  | SourceStep(array<Protocol.manager>)
  | SigningStep(Account.t, array<Protocol.manager>, option<Umami.Protocol.Simulation.results>)

module Content = {
  @react.component
  let make = (~account, ~dryRun, ~operations, ~resetGlobalBatch, ~closeAction) => {
    let (_, sendOperationSimulate) = StoreContext.Operations.useSimulate()
    let (step, setStep) = React.useState(() =>
      switch Alias.toAccount(account) {
      | Ok(account) => SigningStep(account, operations, dryRun)
      | Error(_) => SourceStep(operations)
      }
    )
    switch step {
    | SigningStep(source, operations, dryRun) =>
      <SignStep source dryRun operations resetGlobalBatch closeAction />
    | SourceStep(operations) =>
      module PM = ProtocolHelper.Multisig
      let onSubmit = (source: Account.t) => {
        let operations =
          operations
          ->ProtocolHelper.Multisig.batch
          ->ProtocolHelper.Multisig.propose(account.Alias.address)
        sendOperationSimulate(source, operations)->Promise.getOk(dryRun => {
          setStep(_ => SigningStep(source, operations, Some(dryRun)))
        })
      }
      <ModalFormView
        title=I18n.Title.submit_batch closing={ModalFormView.Close(_ => closeAction())}>
        <SourceStepView multisig=account.address sender=account onSubmit />
      </ModalFormView>
    }
  }
}

@react.component
let make = (~account: Alias.t, ~dryRun, ~operations, ~resetGlobalBatch) => {
  let (operationSimulateRequest, _) = StoreContext.Operations.useSimulate()
  let (openAction, closeAction, wrapModal) = ModalAction.useModal()
  let disabled = operations == []
  let onPress = _ => openAction()
  let loading = ApiRequest.isLoading(operationSimulateRequest)
  <>
    <Buttons.SubmitPrimary text=I18n.Btn.batch_submit onPress disabled loading />
    {wrapModal(<Content account dryRun operations resetGlobalBatch closeAction />)}
  </>
}
