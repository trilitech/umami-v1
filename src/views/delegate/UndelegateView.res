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

let styles = {
  open Style
  StyleSheet.create({
    "deleteLoading": style(~paddingVertical=170.->dp, ()),
  })
}

type step =
  | SendStep
  | SourceStep
  | SigningStep(Account.t, array<Protocol.manager>, Protocol.Simulation.results)
  | SubmittedStep(string)

let stepToString = step =>
  switch step {
  | SendStep => "sendstep"
  | SourceStep => "sourcestep"
  | SigningStep(_) => "signingstep"
  | SubmittedStep(_) => "submittedstep"
  }

@react.component
let make = (~closeAction, ~account, ~delegate) => {
  let (modalStep, setModalStep) = React.useState(_ => SendStep)
  let (_, setStack) as stackState = React.useState(_ => list{})
  let (signStep, _) as signOpStep = React.useState(() => SignOperationView.SummaryStep)

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate()

  let sendOperation = (~operation, signingIntent) =>
    sendOperation({operation: operation, signingIntent: signingIntent})->Promise.tapOk(result =>
      setModalStep(_ => SubmittedStep(result.hash))
    )

  let (_, sendOperationSimulate) = StoreContext.Operations.useSimulate()

  let makeOperation = () =>
    {
      open Protocol.Delegation
      {
        delegate: Undelegate(Some(delegate)),
        options: ProtocolOptions.make(),
      }
    }->Protocol.Delegation

  React.useEffect0(() => {
    switch account.Alias.kind {
    | Some(Account(_)) =>
      let account = Alias.toAccountExn(account)
      let operations = [makeOperation()]
      sendOperationSimulate(account, operations)->Promise.getOk(dryRun =>
        setModalStep(_ => SigningStep(account, operations, dryRun))
      )
    | Some(Multisig) =>
      let action = [makeOperation()]
      let initiator = account.address
      setStack(_ => list{(initiator, action, None)})
      setModalStep(_ => SourceStep)
    | _ => assert false
    }
    None
  })

  let title = switch modalStep {
  | SigningStep(_) =>
    SignOperationView.makeTitle(~custom=I18n.Title.delegate_delete, signStep)->Some
  | SourceStep | SendStep => I18n.Title.delegate_delete->Some
  | SubmittedStep(_) => None
  }

  let closing = ModalFormView.Close(closeAction)

  let back = SignOperationView.back(signOpStep, () =>
    switch modalStep {
    | SourceStep
    | SigningStep(_) =>
      let default = () => setModalStep(_ => SendStep)
      SourceStepView.back(~default, stackState)
    | _ => None
    }
  )

  <ReactFlipToolkit.Flipper flipKey={modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView back closing ?title>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch modalStep {
          | SubmittedStep(hash) => <SubmittedView hash onPressCancel={_ => closeAction()} />
          | SendStep => <LoadingView style={styles["deleteLoading"]} />
          | SourceStep =>
            let callback = (account, operations) =>
              sendOperationSimulate(account, operations)->Promise.getOk(dryRun => {
                setModalStep(_ => SigningStep(account, operations, dryRun))
              })
            <SourceStepView stack=stackState callback />
          | SigningStep(signer, operations, dryRun) =>
            let loading = operationRequest->ApiRequest.isLoading
            <SignOperationView signer signOpStep dryRun operations sendOperation loading />
          }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>
}
