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

open ReactNative;

module FormGroupAmountWithTokenSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~flexDirection=`row, ()),
        "amountGroup": style(~flexGrow=1., ()),
        "tokenGroup": style(~marginLeft=14.->dp, ()),
        "tokenSelector": style(~width=100.->dp, ~marginBottom=0.->dp, ()),
      })
    );
  let tokenDecoration = (~symbol, ~style) =>
    <Typography.Body1 style> symbol->React.string </Typography.Body1>;

  [@react.component]
  let make =
      (
        ~label,
        ~value,
        ~handleChange,
        ~error,
        ~selectedToken,
        ~setSelectedToken,
        ~token: option(Token.t)=?,
      ) => {
    let tokens = StoreContext.Tokens.useGetAll();

    let displaySelector = tokens->Map.String.size > 0;

    let decoration =
      switch (displaySelector, token) {
      | (true, _) => None
      | (false, None) => Some(FormGroupCurrencyInput.tezDecoration)
      | (false, Some(token)) => Some(tokenDecoration(~symbol=token.symbol))
      };

    <View style=styles##container>
      <FormGroupCurrencyInput
        style=styles##amountGroup
        label
        value
        handleChange
        error
        ?decoration
        ?token
      />
      {<FormGroup style=styles##tokenGroup>
         <FormLabel
           label=I18n.label#token
           style=FormGroupTextInput.styles##label
         />
         <TokenSelector
           selectedToken
           setSelectedToken
           style=styles##tokenSelector
         />
       </FormGroup>
       ->ReactUtils.onlyWhen(displaySelector)}
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({"operationSummary": style(~marginBottom=20.->dp, ())})
  );

type step =
  | SendStep
  | PasswordStep(SendForm.transaction, Protocol.simulationResults)
  | EditStep(int, (SendForm.validState, bool))
  | BatchStep
  | SubmittedStep(string);

let stepToString = step =>
  switch (step) {
  | SendStep => "sendstep"
  | PasswordStep(_, _) => "passwordstep"
  | EditStep(_, _) => "editstep"
  | BatchStep => "batchstep"
  | SubmittedStep(_) => "submittedstep"
  };

module Form = {
  let defaultInit = (account: option(Account.t)) =>
    SendForm.StateLenses.{
      amount: "",
      sender: account,
      recipient: FormUtils.Alias.AnyString(""),
      fee: "",
      gasLimit: "",
      storageLimit: "",
      forceLowFee: false,
      dryRun: None,
    };

  let use = (~initValues=?, initAccount, token, onSubmit) => {
    SendForm.use(
      ~schema={
        SendForm.Validation.(
          Schema(
            custom(values => values.sender->FormUtils.notNone, Sender)
            + custom(
                values =>
                  switch (values.recipient) {
                  | AnyString(_) =>
                    Error(I18n.form_input_error#invalid_contract)
                  | Valid(Alias(_)) => Valid
                  | Valid(Address(_)) => Valid
                  },
                Recipient,
              )
            + custom(
                values => {
                  token != None
                    ? FormUtils.isValidTokenAmount(values.amount)
                    : FormUtils.isValidTezAmount(values.amount)
                },
                Amount,
              )
            + custom(
                values => FormUtils.(emptyOr(isValidTezAmount, values.fee)),
                Fee,
              )
            + custom(
                values => FormUtils.isValidInt(values.gasLimit),
                GasLimit,
              )
            + custom(
                values => FormUtils.isValidInt(values.storageLimit),
                StorageLimit,
              ),
          )
        );
      },
      ~onSubmit=
        f => {
          onSubmit(f);
          None;
        },
      ~initialState=
        initValues->Option.getWithDefault(defaultInit(initAccount)),
      ~i18n=FormUtils.i18n,
      (),
    );
  };

  module View = {
    open SendForm;

    let onAppear = (el, _) => {
      ReactFlipToolkit.spring({
        onUpdate: value => {
          el->ReactDOMRe.domElementToObj##style##opacity #= value;
        },
        delay: 100.,
        onComplete: () => (),
      });
    };

    let onExit = (el, _, removeElement) => {
      ReactFlipToolkit.spring({
        onUpdate: value => {
          el->ReactDOMRe.domElementToObj##style##opacity
          #= Js.Math.max_float(0., 1. -. value -. 0.3);
        },
        delay: 0.,
        onComplete: removeElement,
      });
    };

    type mode =
      | Edition(int)
      | Creation(option(unit => unit), unit => unit);

    let simulatedTransaction = (mode, batch, state: SendForm.validState) => {
      let (batch, index) =
        switch (mode) {
        | Edition(index) =>
          let batch =
            batch
            ->List.mapWithIndex((id, elt) =>
                id == index ? (state, true) : elt
              )
            ->List.reverse;
          (batch, Some(batch->List.length - (index + 1)));

        | Creation(_) =>
          let batch = [(state, true), ...batch]->List.reverse;
          let length = batch->List.length;

          (batch, Some(length - 1));
        };

      SendForm.buildTransaction(batch)
      ->Operation.Simulation.transaction(index);
    };

    [@react.component]
    let make =
        (
          ~batch,
          ~advancedOptionState,
          ~tokenState: (
             option(TezosClient.Token.t),
             (option(TezosClient.Token.t) => option(TezosClient.Token.t)) =>
             unit,
           ),
          ~token=?,
          ~mode,
          ~form,
          ~aliases,
          ~loading,
        ) => {
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;
      let (selectedToken, setSelectedToken) = tokenState;

      let (editing, onAddToBatch, onSubmitAll, batchMode) =
        switch (mode) {
        // Edition state is always on batch mode
        | Edition(_) => (true, None, None, true)
        | Creation(batch, submit) => (
            false,
            batch,
            Some(submit),
            batch == None,
          )
        };

      let submitLabel =
        editing
          ? I18n.btn#update
          : batchMode ? I18n.btn#add_transaction : I18n.btn#send_submit;

      let onSubmit = onSubmitAll->Option.getWithDefault(() => form.submit());

      let formFieldsAreValids =
        FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

      let advancedOptionsDisabled =
        form.getFieldState(Field(Amount)) != Valid
        || form.getFieldState(Field(Sender)) != Valid
        && form.getFieldState(Field(Sender)) != Pristine
        || form.getFieldState(Field(Recipient)) != Valid;

      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <View style=FormStyles.header>
            <Typography.Headline>
              I18n.title#send->React.string
            </Typography.Headline>
            <Typography.Overline1 style=FormStyles.subtitle>
              I18n.title#send_many_transactions->React.string
            </Typography.Overline1>
            <Typography.Body2 style=FormStyles.subtitle>
              I18n.expl#send_many_transactions->React.string
            </Typography.Body2>
          </View>
          <FormGroupAmountWithTokenSelector
            label=I18n.label#send_amount
            value={form.values.amount}
            handleChange={form.handleChange(Amount)}
            error={form.getFieldError(Field(Amount))}
            selectedToken={selectedToken->Option.map(t => t.address)}
            setSelectedToken={newToken => setSelectedToken(_ => newToken)}
            ?token
          />
          <FormGroupAccountSelector
            disabled=batchMode
            label=I18n.label#send_sender
            value={form.values.sender}
            handleChange={form.handleChange(Sender)}
            error={form.getFieldError(Field(Sender))}
            ?token
          />
          <FormGroupContactSelector
            label=I18n.label#send_recipient
            filterOut={form.values.sender->Option.map(Account.toAlias)}
            aliases
            value={form.values.recipient}
            handleChange={form.handleChange(Recipient)}
            error={form.getFieldError(Field(Recipient))}
          />
          <SwitchItem
            label=I18n.btn#advanced_options
            value=advancedOptionOpened
            setValue=setAdvancedOptionOpened
            disabled=advancedOptionsDisabled
          />
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView
          flipId="advancedOption"
          scale=false
          translate=advancedOptionOpened
          opacity=true>
          <ReactFlipToolkit.Flipper
            flipKey={advancedOptionOpened->string_of_bool}>
            {advancedOptionOpened
               ? <ReactFlipToolkit.FlippedView
                   flipId="innerAdvancedOption" onAppear onExit>
                   <SendViewAdvancedOptions
                     token
                     operation={simulatedTransaction(
                       mode,
                       batch,
                       SendForm.unsafeExtractValidState(token, form.values),
                     )}
                     form
                   />
                 </ReactFlipToolkit.FlippedView>
               : React.null}
          </ReactFlipToolkit.Flipper>
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView flipId="submit">
          <View style=FormStyles.verticalFormAction>
            <Buttons.SubmitPrimary
              text=submitLabel
              onPress={_ => onSubmit()}
              loading
              disabledLook={!formFieldsAreValids}
            />
            {onAddToBatch->ReactUtils.mapOpt(addToBatch =>
               <Buttons.FormSecondary
                 style=FormStyles.formSecondary
                 text=I18n.btn#start_batch_transaction
                 onPress={_ => addToBatch()}
               />
             )}
          </View>
        </ReactFlipToolkit.FlippedView>
      </>;
    };
  };
};

module EditionView = {
  [@react.component]
  let make = (~batch, ~aliases, ~initValues, ~onSubmit, ~index, ~loading) => {
    let (initValues, advancedOptionOpened) = initValues;
    let token =
      switch (initValues.SendForm.amount) {
      | Transfer.Currency.Tez(_) => None
      | Token(_, t) => Some(t)
      };
    let initValues = initValues->SendForm.toState;

    let (advancedOptionOpened, _) as advancedOptionState =
      React.useState(_ => advancedOptionOpened);

    let form =
      Form.use(~initValues, None, token, onSubmit(advancedOptionOpened));

    <Form.View
      batch
      advancedOptionState
      tokenState=(token, _ => ())
      ?token
      form
      mode={Form.View.Edition(index)}
      aliases
      loading
    />;
  };
};

[@react.component]
let make = (~closeAction) => {
  let account = StoreContext.SelectedAccount.useGet();
  let initToken = StoreContext.SelectedToken.useGet();
  let aliasesRequest = StoreContext.Aliases.useRequest();

  let aliases =
    aliasesRequest
    ->ApiRequest.getDoneOk
    ->Option.getWithDefault(Map.String.empty);

  let updateAccount = StoreContext.SelectedAccount.useSet();

  let (advancedOptionsOpened, setAdvancedOptions) as advancedOptionState =
    React.useState(_ => false);

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let (selectedToken, _) as tokenState =
    React.useState(_ => initToken->Option.map(initToken => initToken));
  let token =
    StoreContext.Tokens.useGet(selectedToken->Option.map(t => t.address));

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendTransfer = (~transfer, ~password) => {
    let operation = Operation.transfer(transfer);

    sendOperation({operation, password})
    ->Future.tapOk(hash => {setModalStep(_ => SubmittedStep(hash))})
    ->Future.tapOk(_ => {updateAccount(transfer.source)});
  };

  let (batch, setBatch) = React.useState(_ => []);

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let submitAction = React.useRef(`SubmitAll);

  let onSubmitBatch = batch => {
    let transaction = SendForm.buildTransaction(batch->List.reverse);
    sendOperationSimulate(
      Operation.Simulation.transaction(transaction, None),
    )
    ->Future.tapOk(dryRun => {
        setModalStep(_ => PasswordStep(transaction, dryRun))
      })
    ->ignore;
  };

  let onSubmit = ({state, send}: SendForm.onSubmitAPI) => {
    let validState = SendForm.unsafeExtractValidState(token, state.values);
    switch (submitAction.current) {
    | `SubmitAll =>
      onSubmitBatch([(validState, advancedOptionsOpened), ...batch])
    | `AddToBatch =>
      setBatch(l => [(validState, advancedOptionsOpened), ...l]);
      setAdvancedOptions(_ => false);
      send(ResetForm);
      send(SetFieldValue(Sender, state.values.sender));
    };
  };

  let form: SendForm.api = Form.use(account, token, onSubmit);

  let onSubmitAll = _ => {
    submitAction.current = `SubmitAll;
    form.submit();
  };

  let onAddToBatch = _ => {
    submitAction.current = `AddToBatch;
    form.submit();
    setModalStep(_ => BatchStep);
  };

  let onAddCSVList = (csvRows: CSVEncoding.t) => {
    let transformTransfer =
      csvRows->List.mapReverse(({destination, amount}) => {
        let formStateValues: SendForm.validState = {
          amount,
          sender: form.values.sender->FormUtils.Unsafe.getValue,
          recipient: FormUtils.Alias.Address(destination),
          fee: None,
          gasLimit: None,
          storageLimit: None,
          forceLowFee: false,
          dryRun: None,
        };
        (formStateValues, false);
      });
    setBatch(_ => transformTransfer);
  };

  let onEdit = (i, advOpened, {state}: SendForm.onSubmitAPI) => {
    let validState = SendForm.unsafeExtractValidState(token, state.values);
    setBatch(b =>
      b->List.mapWithIndex((j, v) => i == j ? (validState, advOpened) : v)
    );
    setModalStep(_ => BatchStep);
  };

  let onDelete = i => {
    setBatch(b => b->List.keepWithIndex((_, j) => j != i));
    List.length(batch) == 1 ? setModalStep(_ => SendStep) : ();
  };

  let closing =
    switch (form.formState, modalStep) {
    | (Pristine, _) when batch == [] => ModalFormView.Close(closeAction)
    | (_, SubmittedStep(_)) => ModalFormView.Close(closeAction)
    | _ =>
      ModalFormView.confirm(~actionText=I18n.btn#send_cancel, closeAction)
    };

  let back =
    switch (modalStep) {
    | PasswordStep(_, _) =>
      Some(() => setModalStep(_ => batch == [] ? SendStep : BatchStep))
    | EditStep(_, _) => Some(() => setModalStep(_ => BatchStep))
    | SendStep => batch != [] ? Some(_ => setModalStep(_ => BatchStep)) : None
    | _ => None
    };

  let onPressCancel = _ => {
    closeAction();
    Routes.(push(Operations));
  };

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading;
  let loading = operationRequest->ApiRequest.isLoading;

  <ReactFlipToolkit.Flipper
    flipKey={advancedOptionsOpened->string_of_bool ++ modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView back closing>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch (modalStep) {
           | SubmittedStep(hash) =>
             <SubmittedView
               hash
               onPressCancel
               submitText=I18n.btn#go_operations
             />
           | BatchStep =>
             <BatchView
               onAddTransfer={_ => setModalStep(_ => SendStep)}
               onAddCSVList
               batch
               onSubmitBatch
               onEdit={(i, (state, adv)) =>
                 setModalStep(_ => EditStep(i, (state, adv)))
               }
               onDelete
               loading=loadingSimulate
             />
           | EditStep(index, initValues) =>
             let onSubmit = (advOpened, form) =>
               onEdit(index, advOpened, form);
             <EditionView
               batch
               initValues
               onSubmit
               index
               loading=false
               aliases
             />;
           | SendStep =>
             let onSubmit = batch != [] ? onAddToBatch : onSubmitAll;
             let onAddToBatch = batch != [] ? None : Some(onAddToBatch);
             <Form.View
               batch
               advancedOptionState
               tokenState
               ?token
               form
               mode={Form.View.Creation(onAddToBatch, onSubmit)}
               loading=loadingSimulate
               aliases
             />;
           | PasswordStep(transfer, dryRun) =>
             <SignOperationView
               title=I18n.title#confirmation
               subtitle=I18n.expl#confirm_operation
               sendOperation={sendTransfer(~transfer)}
               loading>
               <OperationSummaryView.Transactions transfer dryRun />
             </SignOperationView>
           }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>;
};
