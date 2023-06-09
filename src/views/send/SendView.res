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

module FormGroupAmountWithTokenSelector = {
  let styles = {
    open Style
    StyleSheet.create({
      "container": style(~flexDirection=#row, ()),
      "amountGroup": style(~flexGrow=1., ()),
      "tokenGroup": style(~marginLeft=14.->dp, ()),
      "tokenSelector": style(~width=100.->dp, ~marginBottom=0.->dp, ()),
    })
  }
  let tokenDecoration = (~symbol, ~style) =>
    <Typography.Body1 style> {symbol->React.string} </Typography.Body1>

  @react.component
  let make = (
    ~label,
    ~value,
    ~handleChange,
    ~error,
    ~selectedToken,
    ~setSelectedToken,
    ~token: option<Token.t>=?,
  ) => {
    let tokens = StoreContext.Tokens.useGetAllFungible()

    let tokens = React.useMemo1(
      () => tokens->TokensLibrary.Generic.keepTokens((_, _, (_, registered)) => registered),
      [tokens],
    )

    let displaySelector = !(tokens->TokensLibrary.Contracts.isEmpty)

    let decoration = switch (displaySelector, token) {
    | (true, _) => None
    | (false, None) => Some(FormGroupCurrencyInput.tezDecoration)
    | (false, Some(token)) => Some(tokenDecoration(~symbol=token.symbol))
    }

    <View style={styles["container"]}>
      <FormGroupCurrencyInput
        style={styles["amountGroup"]} label value handleChange error ?decoration ?token
      />
      {<FormGroup style={styles["tokenGroup"]}>
        <FormLabel label=I18n.Label.token style={FormGroupTextInput.styles["label"]} />
        <TokenSelector selectedToken setSelectedToken style={styles["tokenSelector"]} />
      </FormGroup>->ReactUtils.onlyWhen(displaySelector)}
    </View>
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

module Form = {
  let defaultInit = (account: Alias.t) => {
    open SendForm.StateLenses
    {
      amount: "",
      sender: account,
      recipient: FormUtils.Alias.AnyString(""),
    }
  }

  let use = (~initValues=?, initAccount, token: option<TokenRepr.t>, onSubmit) =>
    SendForm.use(
      ~schema={
        open SendForm.Validation
        Schema(custom(values =>
            switch values.recipient {
            | AnyString(_) => Error(I18n.Form_input_error.invalid_contract)
            | Temp(_, Pending | NotAsked) => Error("")
            | Temp(_, Error(s)) => Error(s)
            | Valid(Alias(_)) => Valid
            | Valid(Address(_)) => Valid
            }
          , Recipient) + custom(values =>
            switch token {
            | None => FormUtils.isValidTezAmount(values.amount)
            | Some({decimals, _}) => FormUtils.isValidTokenAmount(values.amount, decimals)
            }
          , Amount))
      },
      ~onSubmit=f => {
        onSubmit(f)
        None
      },
      ~initialState=initValues->Option.getWithDefault(defaultInit(initAccount)),
      ~i18n=FormUtils.i18n,
      (),
    )

  module View = {
    open SendForm

    type mode =
      | Edition(int)
      | Creation(option<unit => unit>, unit => unit)

    @react.component
    let make = (
      ~proposal=false,
      ~tokenState: (option<Token.t>, (option<Token.t> => option<Token.t>) => unit),
      ~token=?,
      ~mode,
      ~form,
      ~loading,
      ~simulatingBatch=false,
    ) => {
      let (selectedToken, setSelectedToken) = tokenState

      let (editing, onAddToBatch, onSubmitAll, batchMode) = switch mode {
      // Edition state is always on batch mode
      | Edition(_) => (true, None, None, true)
      | Creation(batch, submit) => (false, batch, Some(submit), batch == None)
      }

      let submitLabel = editing
        ? I18n.Btn.update
        : batchMode
        ? I18n.Btn.add_transaction
        : proposal
        ? I18n.Btn.proposal_submit
        : I18n.Btn.send_submit

      let onSubmit = onSubmitAll->Option.getWithDefault(() => form.submit())

      let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <View style=FormStyles.header>
            <Typography.Overline1>
              {I18n.Title.send_many_transactions->React.string}
            </Typography.Overline1>
          </View>
          <FormGroupAmountWithTokenSelector
            label=I18n.Label.send_amount
            value=form.values.amount
            handleChange={form.handleChange(Amount)}
            error={form.getFieldError(Field(Amount))}
            selectedToken
            setSelectedToken={newToken => setSelectedToken(_ => newToken)}
            ?token
          />
          <FormGroupAccountSelector
            disabled=batchMode
            label=I18n.Label.send_sender
            value=form.values.sender
            handleChange={form.handleChange(Sender)}
          />
          <FormGroupContactSelector
            label=I18n.Label.send_recipient
            keep={a => a.address != form.values.sender.address}
            value=form.values.recipient
            onChange={form.handleChange(Recipient)}
            error={form.getFieldError(Field(Recipient))}
          />
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView flipId="submit">
          <View style=FormStyles.verticalFormAction>
            <Buttons.SubmitPrimary
              text=submitLabel onPress={_ => onSubmit()} loading disabled={!formFieldsAreValids}
            />
            {onAddToBatch->ReactUtils.mapOpt(addToBatch =>
              <Buttons.SubmitSecondary
                loading=simulatingBatch
                style=FormStyles.formSecondary
                text=I18n.global_batch_add
                onPress={_ => addToBatch()}
                disabled={!formFieldsAreValids}
              />
            )}
          </View>
        </ReactFlipToolkit.FlippedView>
      </>
    }
  }
}

module EditionView = {
  @react.component
  let make = (~closeAction, ~account, ~initValues, ~onSubmit, ~index, ~loading) => {
    let token = switch initValues.SendForm.amount {
    | Protocol.Amount.Tez(_) => None
    | Token({token}) => Some(token)
    }
    let (token, _) as tokenState = React.useState(() => token)
    let onSubmit = (_, {state}: SendForm.onSubmitAPI) => {
      let validState = SendForm.unsafeExtractValidState(token, state.values)
      onSubmit(validState)
    }
    let initValues = initValues->SendForm.toState
    let form = Form.use(~initValues, account, token, onSubmit(token))
    let closing = ModalFormView.confirm(~actionText=I18n.Btn.update_cancel, closeAction)
    let title = I18n.Title.send
    <ModalFormView title /* back=None */ closing titleStyle=FormStyles.headerMarginBottom8>
      <Form.View tokenState ?token form mode=Form.View.Edition(index) loading />
    </ModalFormView>
  }
}

let senderIsMultisig = (sender: Alias.t) => PublicKeyHash.isContract(sender.address)

@react.component
let make = (~account, ~closeAction, ~initalStep=SendStep) => {
  let initToken = StoreContext.SelectedToken.useGet()

  let updateAccount = StoreContext.SelectedAccount.useSet()

  let (modalStep, setModalStep) = React.useState(_ => initalStep)

  let (token, _) as tokenState = React.useState(_ => initToken->Option.map(initToken => initToken))

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate()

  let sendOperation = (~sender: Alias.t, ~operation: Protocol.batch, signingIntent) =>
    sendOperation({operation: operation, signingIntent: signingIntent})
    ->Promise.tapOk(result => setModalStep(_ => SubmittedStep(result.hash)))
    ->Promise.tapOk(_ => updateAccount(sender.Alias.address))

  let (batch, _) = React.useState(_ => list{})
  let (_, setStack) as stackState = React.useState(_ => list{})

  let (operationSimulateRequest, sendOperationSimulate) = StoreContext.Operations.useSimulate()

  let submitAction = React.useRef(#SubmitAll)

  let onSubmitBatch = (account, batch) => {
    let transaction = SendForm.buildTransactions(batch)
    sendOperationSimulate(account, transaction)->Promise.getOk(dryRun => {
      setModalStep(_ => SigningStep(account, transaction, dryRun))
    })
  }

  let onSubmitMultisig = (state: SendForm.validState) => {
    let action = [SendForm.buildTransaction(state)]
    let initiator = state.sender.Alias.address
    let state = (initiator, action, None)
    setStack(_ => list{state})
    setModalStep(_ => SourceStep)
  }

  let {addTransfer, isSimulating} = GlobalBatchContext.useGlobalBatchContext()

  let onSubmit = ({state}: SendForm.onSubmitAPI) => {
    let validState = SendForm.unsafeExtractValidState(token, state.values)

    switch submitAction.current {
    | #SubmitAll =>
      switch state.values.sender.kind {
      | Some(Alias.Multisig) => onSubmitMultisig(validState)
      | Some(Account(_)) =>
        onSubmitBatch(state.values.sender->Alias.toAccountExn, list{validState, ...batch})
      | _ => assert false
      }
    | #AddToBatch =>
      let p = GlobalBatchXfs.validStateToTransferPayload(validState)
      addTransfer(validState.sender.address, p, closeAction)
    }
  }

  let form: SendForm.api = Form.use(account, token, onSubmit)

  let onSubmitAll = _ => {
    submitAction.current = #SubmitAll
    form.submit()
  }

  let onAddToBatch = _ => {
    submitAction.current = #AddToBatch
    form.submit()
  }

  let (signerState, _) as state = React.useState(() => None)

  let (sign, setSign) as signOpStep = React.useState(() => SignOperationView.SummaryStep)

  let closing = switch sign {
  | AdvancedOptStep(_) => None
  | SummaryStep =>
    switch (form.formState, modalStep, (signerState: option<SigningBlock.state>)) {
    | (_, SigningStep({kind: Ledger}, _, _), Some(WaitForConfirm)) =>
      ModalFormView.Deny(I18n.Tooltip.reject_on_ledger)->Some

    | (_, SigningStep({kind: CustomAuth({provider})}, _, _), Some(WaitForConfirm)) =>
      ModalFormView.Deny(
        I18n.Tooltip.reject_on_provider(provider->ReCustomAuth.getProviderName),
      )->Some

    | (Pristine, _, _) if batch == list{} => ModalFormView.Close(closeAction)->Some
    | (_, SubmittedStep(_), _) => ModalFormView.Close(closeAction)->Some
    | _ => ModalFormView.confirm(~actionText=I18n.Btn.send_cancel, closeAction)->Some
    }
  }

  let back = {
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
  }

  let onPressCancel = _ => {
    closeAction()
    open Routes
    push(Operations)
  }

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading
  let loading = operationRequest->ApiRequest.isLoading

  let title = switch modalStep {
  | SendStep
  | SourceStep =>
    Some(I18n.Title.send)
  | SigningStep(_) => SignOperationView.makeTitle(sign)->Some
  | SubmittedStep(_) => None
  }

  <ReactFlipToolkit.Flipper flipKey={modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView ?title back ?closing titleStyle=FormStyles.headerMarginBottom8>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch modalStep {
          | SubmittedStep(hash) =>
            <SubmittedView hash onPressCancel submitText=I18n.Btn.go_operations />
          | SendStep =>
            <Form.View
              proposal={senderIsMultisig(form.state.values.sender)}
              tokenState
              ?token
              form
              mode=Form.View.Creation(Some(onAddToBatch), onSubmitAll)
              loading=loadingSimulate
              simulatingBatch=isSimulating
            />
          | SigningStep(source, operations, dryRun) =>
            <SignOperationView
              proposal={senderIsMultisig(form.state.values.sender)}
              signer=source
              state
              signOpStep
              dryRun
              operations
              sendOperation={sendOperation(~sender=form.state.values.sender)}
              loading
            />
          | SourceStep =>
            let callback = (account, operations) =>
              sendOperationSimulate(account, operations)->Promise.getOk(dryRun => {
                setModalStep(_ => SigningStep(account, operations, dryRun))
              })
            <SourceStepView ?back stack=stackState callback />
          }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>
}
