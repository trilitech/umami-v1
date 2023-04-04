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

module Form = {
  let defaultInit = () => {
    open SendNFTForm.StateLenses
    {recipient: FormUtils.Alias.AnyString("")}
  }

  let use = (~initValues=?, onSubmit) =>
    SendNFTForm.use(
      ~schema={
        open SendNFTForm.Validation
        Schema(custom(values =>
            switch values.recipient {
            | Temp(_, Pending | NotAsked) => Error("")
            | Temp(_, Error(s)) => Error(s)
            | AnyString(_) => Error(I18n.Form_input_error.invalid_contract)
            | Valid(Alias(_)) => Valid
            | Valid(Address(_)) => Valid
            }
          , Recipient))
      },
      ~onSubmit=f => {
        onSubmit(f)
        None
      },
      ~initialState=initValues->Option.getWithDefault(defaultInit()),
      ~i18n=FormUtils.i18n,
      (),
    )

  module FormGroupNFTView = {
    let styles = {
      open Style
      StyleSheet.create({
        "formGroup": style(~marginBottom=20.->dp, ()),
        "label": style(~marginBottom=6.->dp, ()),
        "input": style(~paddingHorizontal=20.->dp, ~fontWeight=#bold, ()),
      })
    }

    @react.component
    let make = (~nft: Token.t) =>
      <FormGroup style={styles["formGroup"]}>
        <FormLabel label=I18n.Label.send_nft hasError=false style={styles["label"]} />
        <View>
          <ThemedTextInput
            style={styles["input"]}
            value=nft.alias
            hasError=false
            onValueChange={_ => ()}
            onFocus={_ => ()}
            onBlur={_ => ()}
            onKeyPress={_ => ()}
            disabled=true
          />
        </View>
      </FormGroup>
  }

  module View = {
    open SendNFTForm

    @react.component
    let make = (
      ~proposal=false,
      ~sender,
      ~nft,
      ~form,
      ~loading,
      ~onAddToBatch,
      ~simulatingBatch=false,
    ) => {
      let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

      let submitLabel = proposal ? I18n.Btn.proposal_submit : I18n.Btn.send_submit

      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <FormGroupAccountSelector
            disabled=true label=I18n.Label.send_sender value=sender handleChange={_ => ()}
          />
          <FormGroupNFTView nft />
          <FormGroupContactSelector
            label=I18n.Label.send_recipient
            keep={a => a.address != sender.address}
            value=form.values.recipient
            onChange={form.handleChange(Recipient)}
            error={form.getFieldError(Field(Recipient))}
          />
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView flipId="submit">
          <View style=FormStyles.verticalFormAction>
            <Buttons.SubmitPrimary
              text=submitLabel onPress={_ => form.submit()} loading disabled={!formFieldsAreValids}
            />
          </View>
          <Buttons.SubmitSecondary
            style=FormStyles.formSecondary
            text=I18n.global_batch_add
            onPress={_ => onAddToBatch()}
            loading=simulatingBatch
            disabled={!formFieldsAreValids}
          />
        </ReactFlipToolkit.FlippedView>
      </>
    }
  }
}

type step =
  | SendStep
  | SigningStep(Account.t, array<Protocol.manager>, Protocol.Simulation.results)
  | SubmittedStep(string)
  | SourceStep

let stepToString = step =>
  switch step {
  | SendStep => "sendstep"
  | SigningStep(_) => "signingstep"
  | SubmittedStep(_) => "submittedstep"
  | SourceStep => "sourcestep"
  }

let unsafeExtractValidState = (
  state: SendNFTForm.state,
  nft,
  source: Alias.t,
): SendForm.validState => {
  let recipient = state.values.recipient->FormUtils.Unsafe.account

  let amount = {
    open Protocol.Amount
    Token({amount: TokenRepr.Unit.one, token: nft, source: source.address})
  }

  {amount: amount, sender: source, recipient: recipient, parameter: None, entrypoint: None}
}

let senderIsMultisig = (sender: Alias.t) => PublicKeyHash.isContract(sender.address)

@react.component
let make = (~source: Alias.t, ~nft: Token.t, ~closeAction) => {
  let (modalStep, setModalStep) = React.useState(_ => SendStep)

  let {addTransfer, isSimulating} = GlobalBatchContext.useGlobalBatchContext()

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate()
  let (operationSimulateRequest, sendOperationSimulate) = StoreContext.Operations.useSimulate()

  let sendTransfer = (~operation, signingIntent) => {
    sendOperation({operation: operation, signingIntent: signingIntent})->Promise.tapOk(result =>
      setModalStep(_ => SubmittedStep(result.hash))
    )
  }

  let isForGlobalBatch = React.useRef(false)
  let (_, setStack) as stackState = React.useState(_ => list{})

  let (sign, setSign) as signOpStep = React.useState(() => SignOperationView.SummaryStep)

  let makeOperation = (state: SendForm.validState) =>
    ProtocolHelper.Transfer.makeSimpleToken(
      ~source=source.address,
      ~destination=state.recipient->FormUtils.Alias.address,
      ~amount=TokenRepr.Unit.one,
      ~token=nft,
      (),
    )->Protocol.Transfer

  let onSubmitImplicit = (state: SendForm.validState) => {
    let account = source->Alias.toAccountExn
    let managers = [makeOperation(state)]
    sendOperationSimulate(account, managers)->Promise.getOk(dryRun =>
      setModalStep(_ => SigningStep(account, managers, dryRun))
    )
  }

  let onSubmitMultisig = (state: SendForm.validState) => {
    let action = [makeOperation(state)]
    let initiator = state.sender.Alias.address
    let state = (initiator, action, None)
    setStack(_ => list{state})
    setModalStep(_ => SourceStep)
  }

  let nominalSubmit = (state: SendForm.validState) =>
    switch source.kind {
    | Some(Alias.Multisig) => onSubmitMultisig(state)
    | Some(Account(_)) => onSubmitImplicit(state)
    | _ => assert false
    }

  let onSubmit = ({state, _}: SendNFTForm.onSubmitAPI) => {
    let validState = unsafeExtractValidState(state, nft, source)
    if isForGlobalBatch.current {
      let p = GlobalBatchXfs.validStateToTransferPayload(validState)
      addTransfer(validState.sender.address, p, closeAction)
    } else {
      nominalSubmit(validState)
    }
  }

  let form: SendNFTForm.api = Form.use(onSubmit)

  let title = switch modalStep {
  | SendStep
  | SourceStep =>
    Some(I18n.Title.send)
  | SigningStep(_) => SignOperationView.makeTitle(sign)->Some
  | SubmittedStep(_) => None
  }

  let back = SignOperationView.back(signOpStep, () => {
    let source_back = () => {
      let default = () => setModalStep(_ => SendStep)
      SourceStepView.back(~default, stackState)
    }
    switch modalStep {
    | SigningStep(_, _, _) =>
      switch sign {
      | AdvancedOptStep(_) => Some(() => setSign(_ => SummaryStep))
      | SummaryStep => source_back()
      }
    | SourceStep => source_back()
    | _ => None
    }
  })

  let closing = Some(ModalFormView.Close(closeAction))

  let signingState = React.useState(() => None)

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading
  let loading = operationRequest->ApiRequest.isLoading

  let addToGlobalBatch = () => {
    isForGlobalBatch.current = true
    form.submit()
  }

  <ReactFlipToolkit.Flipper flipKey={modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView ?title back ?closing titleStyle=FormStyles.headerMarginBottom8>
        {switch modalStep {
        | SendStep =>
          <Form.View
            proposal={senderIsMultisig(source)}
            onAddToBatch=addToGlobalBatch
            sender=source
            nft
            form
            loading=loadingSimulate
            simulatingBatch=isSimulating
          />
        | SigningStep(signer, operations, dryRun) =>
          <SignOperationView
            proposal={senderIsMultisig(source)}
            signer
            state=signingState
            signOpStep
            dryRun
            operations
            sendOperation=sendTransfer
            loading
          />
        | SubmittedStep(hash) =>
          let onPressCancel = _ => {
            closeAction()
            open Routes
            push(Operations)
          }
          <SubmittedView hash onPressCancel submitText=I18n.Btn.go_operations />
        | SourceStep =>
          let callback = (account, operations) =>
            sendOperationSimulate(account, operations)->Promise.getOk(dryRun => {
              setModalStep(_ => SigningStep(account, operations, dryRun))
            })
          <SourceStepView ?back stack=stackState callback />
        }}
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>
}
