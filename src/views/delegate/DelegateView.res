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
open Delegate

let styles = {
  open Style
  StyleSheet.create({
    "switchCmp": style(~height=16.->dp, ~width=32.->dp, ()),
    "switchThumb": style(~transform=[scale(~scale=0.65)], ()),
    "operationSummary": style(~marginBottom=20.->dp, ()),
  })
}

type step =
  | SendStep
  | SourceStep(DelegateForm.state)
  | SigningStep(Account.t, array<Protocol.manager>, Protocol.Simulation.results)
  | SubmittedStep(string)

let stepToString = step =>
  switch step {
  | SendStep => "sendstep"
  | SourceStep(_) => "sourceStep"
  | SigningStep(_) => "signingstep"
  | SubmittedStep(_) => "submittedstep"
  }

module Form = {
  let build = (action: action, onSubmit) => {
    let (initAccount, initDelegate) = switch action {
    | Create(account, _) => (account, None)
    | Edit(account, delegate) => (account, Some(delegate))
    }

    DelegateForm.use(
      ~schema={
        open DelegateForm.Validation
        Schema(custom(values => values.baker->FormUtils.checkAddress, Baker) + custom(values => {
            open FormUtils
            emptyOr(isValidTezAmount, values.fee)
          }, Fee) + custom(values =>
            switch initDelegate {
            | Some(initDelegate) =>
              (initDelegate :> string) == values.baker
                ? Error(I18n.Form_input_error.change_baker)
                : Valid
            | None => Valid
            }
          , Baker))
      },
      ~onSubmit=x => {
        onSubmit(x)
        None
      },
      ~initialState={
        sender: initAccount,
        baker: (initDelegate :> option<string>)->Option.getWithDefault(""),
        fee: "",
      },
      ~i18n=FormUtils.i18n,
      (),
    )
  }

  module ViewBase = {
    type param_FormGroupDelegateSelector = {
      value: PublicKeyHash.t,
      handleChange: Alias.t => unit,
      disabled: bool,
      error: option<string>,
    }
    type param_FormGroupBakerSelector = {
      value: option<string>,
      handleChange: option<string> => unit,
      error: option<string>,
    }
    type param_Submit = {
      text: string,
      onPress: ReactNative.Event.pressEvent => unit,
      disabledLook: bool,
      loading: bool,
    }

    @react.component
    let make = (
      ~delegate: param_FormGroupDelegateSelector,
      ~baker: param_FormGroupBakerSelector,
      ~submit: param_Submit,
    ) => {
      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <FormGroupDelegateSelector
            label=I18n.Label.account_delegate
            value=delegate.value
            handleChange=delegate.handleChange
            error=delegate.error
            disabled=delegate.disabled
          />
          <FormGroupBakerSelector
            label=I18n.Label.baker
            value=baker.value
            handleChange=baker.handleChange
            error=baker.error
          />
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView flipId="submit">
          <View style=FormStyles.verticalFormAction>
            <Buttons.SubmitPrimary
              text=submit.text
              onPress=submit.onPress
              loading=submit.loading
              disabledLook=submit.disabledLook
            />
          </View>
        </ReactFlipToolkit.FlippedView>
      </>
    }
  }

  module View = {
    open DelegateForm

    @react.component
    let make = (~form, ~action, ~loading) => {
      let onSubmitDelegateForm = _ => form.submit()
      let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

      let delegate: ViewBase.param_FormGroupDelegateSelector = {
        value: form.values.sender.address,
        handleChange: {d => form.handleChange(Sender, d)},
        error: {form.getFieldError(Field(Sender))},
        disabled: {
          switch action {
          | Create(_, fixed) => fixed
          | Edit(_) => true
          }
        },
      }
      let baker: ViewBase.param_FormGroupBakerSelector = {
        value: {form.values.baker == "" ? None : form.values.baker->Some},
        handleChange: {b => b->Option.getWithDefault("") |> form.handleChange(Baker)},
        error: {form.getFieldError(Field(Baker))},
      }
      let submit: ViewBase.param_Submit = {
        text: {
          switch action {
          | Create(_) => I18n.Btn.delegation_submit
          | Edit(_) => I18n.Btn.update
          }
        },
        onPress: onSubmitDelegateForm,
        loading: loading,
        disabledLook: {!formFieldsAreValids},
      }
      <ViewBase delegate baker submit />
    }
  }
}

@react.component
let make = (~closeAction, ~action) => {
  let (modalStep, setModalStep) = React.useState(_ => SendStep)

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate()

  let sendOperation = (~operation, signingIntent) =>
    sendOperation({operation: operation, signingIntent: signingIntent})->Promise.tapOk(result =>
      setModalStep(_ => SubmittedStep(result.hash))
    )

  let (operationSimulateRequest, sendOperationSimulate) = StoreContext.Operations.useSimulate()

  let onSubmit = ({state}: DelegateForm.onSubmitAPI) => {
    switch state.values.sender.Alias.kind {
    | Some(Account(_)) =>
      let source = Alias.toAccountExn(state.values.sender)
      let operations = [
        {
          open Protocol.Delegation
          {
            delegate: Delegate(state.values.baker->PublicKeyHash.build->Result.getExn),
            options: ProtocolOptions.make(),
          }
        }->Protocol.Delegation,
      ]
      Promise.async(() =>
        sendOperationSimulate(source, operations)->Promise.mapOk(dryRun =>
          setModalStep(_ => SigningStep(source, operations, dryRun))
        )
      )
    | Some(Multisig) => setModalStep(_ => SourceStep(state))
    | _ => assert false
    }
  }

  let form = Form.build(action, onSubmit)

  let (signStep, _) as signOpStep = React.useState(() => SignOperationView.SummaryStep)

  let title = switch (modalStep, action) {
  | (SigningStep(_), _) =>
    let summaryTitle = I18n.Title.confirm_delegate
    SignOperationView.makeTitle(~custom=summaryTitle, signStep)->Some
  | (SendStep | SourceStep(_), Create(_)) => I18n.Title.delegate->Some
  | (SendStep | SourceStep(_), Edit(_)) => I18n.Title.delegate_update->Some
  | (SubmittedStep(_), _) => None
  }

  let (signingState, _) as state = React.useState(() => None)

  let closing = switch (modalStep, (signingState: option<SigningBlock.state>)) {
  | (SigningStep({kind: Ledger}, _, _), Some(WaitForConfirm)) =>
    ModalFormView.Deny(I18n.Tooltip.reject_on_ledger)
  | (SigningStep({kind: CustomAuth({provider})}, _, _), Some(WaitForConfirm)) =>
    ModalFormView.Deny(I18n.Tooltip.reject_on_provider(provider->ReCustomAuth.getProviderName))
  | _ => ModalFormView.Close(closeAction)
  }

  let back = SignOperationView.back(signOpStep, () =>
    switch modalStep {
    | SigningStep(_) => Some(() => setModalStep(_ => SendStep))
    | _ => None
    }
  )

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading
  let loading = operationRequest->ApiRequest.isLoading

  let onPressCancel = _ => closeAction()

  <ReactFlipToolkit.Flipper flipKey={modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView back closing ?title>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch modalStep {
          | SendStep => <Form.View form action loading=loadingSimulate />
          | SourceStep(state) =>
            let onSubmit = source => {
              let destination = state.values.sender.address
              let parameter = ReTaquito.Toolkit.Lambda.setDelegate(state.values.baker)
              let proposal = ProtocolHelper.Multisig.makeProposal(~parameter, ~destination)
              let operations = [Protocol.Transfer(proposal)]
              sendOperationSimulate(source, operations)->Promise.getOk(dryRun => {
                setModalStep(_ => SigningStep(source, operations, dryRun))
              })
            }
            <SourceStepView
              multisig=state.values.sender.address sender=state.values.sender onSubmit
            />
          | SigningStep(signer, operations, dryRun) =>
            <SignOperationView signer signOpStep dryRun state operations sendOperation loading />
          | SubmittedStep(hash) => <SubmittedView hash onPressCancel />
          }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>
}
