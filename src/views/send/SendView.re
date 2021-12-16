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
    let tokens = StoreContext.Tokens.useGetAll(`FT);

    let tokens =
      React.useMemo1(
        () =>
          tokens->TokensLibrary.Generic.keepTokens((_, _, (_, registered)) =>
            registered
          ),
        [|tokens|],
      );

    let displaySelector = !tokens->TokensLibrary.Contracts.isEmpty;

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
           label=I18n.Label.token
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
  | SigningStep(Transfer.t, Protocol.Simulation.results)
  | EditStep(int, SendForm.validState)
  | BatchStep
  | SubmittedStep(string);

let stepToString = step =>
  switch (step) {
  | SendStep => "sendstep"
  | SigningStep(_, _) => "signingstep"
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
    };

  let use =
      (~initValues=?, initAccount, token: option(TokenRepr.t), onSubmit) => {
    SendForm.use(
      ~schema={
        SendForm.Validation.(
          Schema(
            custom(values => values.sender->FormUtils.notNone, Sender)
            + custom(
                values =>
                  switch (values.recipient) {
                  | AnyString(_) =>
                    Error(I18n.Form_input_error.invalid_contract)
                  | Valid(Alias(_)) => Valid
                  | Valid(Address(_)) => Valid
                  },
                Recipient,
              )
            + custom(
                values => {
                  switch (token) {
                  | None => FormUtils.isValidTezAmount(values.amount)
                  | Some({decimals, _}) =>
                    FormUtils.isValidTokenAmount(values.amount, decimals)
                  }
                },
                Amount,
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

    [@react.component]
    let make =
        (
          ~tokenState: (
             option(Token.t),
             (option(Token.t) => option(Token.t)) => unit,
           ),
          ~token=?,
          ~mode,
          ~form,
          ~aliases,
          ~loading,
        ) => {
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
          ? I18n.Btn.update
          : batchMode ? I18n.Btn.add_transaction : I18n.Btn.send_submit;

      let onSubmit = onSubmitAll->Option.getWithDefault(() => form.submit());

      let formFieldsAreValids =
        FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <View style=FormStyles.header>
            <Typography.Overline1>
              I18n.Title.send_many_transactions->React.string
            </Typography.Overline1>
            <Typography.Body2 style=FormStyles.subtitle>
              I18n.Expl.send_many_transactions->React.string
            </Typography.Body2>
          </View>
          <FormGroupAmountWithTokenSelector
            label=I18n.Label.send_amount
            value={form.values.amount}
            handleChange={form.handleChange(Amount)}
            error={form.getFieldError(Field(Amount))}
            selectedToken
            setSelectedToken={newToken => setSelectedToken(_ => newToken)}
            ?token
          />
          <FormGroupAccountSelector
            disabled=batchMode
            label=I18n.Label.send_sender
            value={form.values.sender}
            handleChange={form.handleChange(Sender)}
            error={form.getFieldError(Field(Sender))}
            ?token
          />
          <FormGroupContactSelector
            label=I18n.Label.send_recipient
            filterOut={form.values.sender->Option.map(Account.toAlias)}
            aliases
            value={form.values.recipient}
            handleChange={form.handleChange(Recipient)}
            error={form.getFieldError(Field(Recipient))}
          />
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
                 text=I18n.Btn.start_batch_transaction
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
  let make = (~aliases, ~initValues, ~onSubmit, ~index, ~loading) => {
    let token =
      switch (initValues.SendForm.amount) {
      | Transfer.Currency.Tez(_) => None
      | Token(_, t) => Some(t)
      };

    let (token, _) as tokenState = React.useState(() => token);

    let initValues = initValues->SendForm.toState;

    let form = Form.use(~initValues, None, token, onSubmit(token));

    <Form.View
      tokenState
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
    ->Option.getWithDefault(PublicKeyHash.Map.empty);

  let updateAccount = StoreContext.SelectedAccount.useSet();

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let (token, _) as tokenState =
    React.useState(_ => initToken->Option.map(initToken => initToken));

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendTransfer =
      (~transfer: Transfer.t, ~operation: Operation.t, signingIntent) => {
    sendOperation({operation, signingIntent})
    ->Promise.tapOk(hash => {setModalStep(_ => SubmittedStep(hash))})
    ->Promise.tapOk(_ => {updateAccount(transfer.source.address)});
  };

  let (batch, setBatch) = React.useState(_ => []);

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let submitAction = React.useRef(`SubmitAll);

  let onSubmitBatch = batch => {
    let transaction = SendForm.buildTransaction(batch);
    sendOperationSimulate(Operation.Simulation.transaction(transaction))
    ->Promise.getOk(dryRun => {
        setModalStep(_ => SigningStep(transaction, dryRun))
      });
  };

  let onSubmit = ({state, send}: SendForm.onSubmitAPI) => {
    let validState = SendForm.unsafeExtractValidState(token, state.values);
    switch (submitAction.current) {
    | `SubmitAll => onSubmitBatch([validState, ...batch])
    | `AddToBatch =>
      setBatch(l => [validState, ...l]);
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
        };
        formStateValues;
      });
    setBatch(_ => transformTransfer);
  };

  let onEdit = (i, token, {state}: SendForm.onSubmitAPI) => {
    let validState = SendForm.unsafeExtractValidState(token, state.values);
    setBatch(b => b->List.mapWithIndex((j, v) => i == j ? validState : v));
    setModalStep(_ => BatchStep);
  };

  let onDelete = i => {
    setBatch(b => b->List.keepWithIndex((_, j) => j != i));
    List.length(batch) == 1 ? setModalStep(_ => SendStep) : ();
  };

  let (ledger, _) as ledgerState = React.useState(() => None);

  let (sign, setSign) as signOpStep =
    React.useState(() => SignOperationView.SummaryStep);

  let closing =
    switch (sign) {
    | AdvancedOptStep(_) => None
    | SummaryStep =>
      switch (
        form.formState,
        modalStep,
        ledger: option(SigningBlock.LedgerView.state),
      ) {
      | (_, SigningStep(_, _), Some(WaitForConfirm)) =>
        ModalFormView.Deny(I18n.Tooltip.reject_on_ledger)->Some

      | (Pristine, _, _) when batch == [] =>
        ModalFormView.Close(closeAction)->Some
      | (_, SubmittedStep(_), _) => ModalFormView.Close(closeAction)->Some
      | _ =>
        ModalFormView.confirm(~actionText=I18n.Btn.send_cancel, closeAction)
        ->Some
      }
    };

  let back =
    switch (sign) {
    | AdvancedOptStep(_) => Some(() => setSign(_ => SummaryStep))
    | SummaryStep =>
      switch (modalStep) {
      | SigningStep(_, _) =>
        Some(() => setModalStep(_ => batch == [] ? SendStep : BatchStep))
      | EditStep(_, _) => Some(() => setModalStep(_ => BatchStep))
      | SendStep =>
        batch != [] ? Some(_ => setModalStep(_ => BatchStep)) : None
      | _ => None
      }
    };

  let onPressCancel = _ => {
    closeAction();
    Routes.(push(Operations));
  };

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading;
  let loading = operationRequest->ApiRequest.isLoading;

  let title =
    switch (modalStep) {
    | SendStep
    | EditStep(_) => Some(I18n.Title.send)
    | BatchStep => Some(I18n.Title.batch)
    | SigningStep(_, _) => SignOperationView.makeTitle(sign)->Some
    | SubmittedStep(_) => None
    };

  <ReactFlipToolkit.Flipper flipKey={modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView
        ?title back ?closing titleStyle=FormStyles.headerMarginBottom8>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch (modalStep) {
           | SubmittedStep(hash) =>
             <SubmittedView
               hash
               onPressCancel
               submitText=I18n.Btn.go_operations
             />
           | BatchStep =>
             <BatchView
               onAddTransfer={_ => setModalStep(_ => SendStep)}
               onAddCSVList
               batch={batch->List.reverse}
               onSubmitBatch
               onEdit={(i, state) =>
                 setModalStep(_ =>
                   EditStep(batch->List.length - i - 1, state)
                 )
               }
               onDelete={i => onDelete(batch->List.length - i - 1)}
               loading=loadingSimulate
             />
           | EditStep(index, initValues) =>
             let onSubmit = form => onEdit(index, form);
             <EditionView initValues onSubmit index loading=false aliases />;
           | SendStep =>
             let onSubmit = batch != [] ? onAddToBatch : onSubmitAll;
             let onAddToBatch = batch != [] ? None : Some(onAddToBatch);
             <Form.View
               tokenState
               ?token
               form
               mode={Form.View.Creation(onAddToBatch, onSubmit)}
               loading=loadingSimulate
               aliases
             />;
           | SigningStep(transfer, dryRun) =>
             <SignOperationView
               source={transfer.source}
               ledgerState
               signOpStep
               dryRun
               subtitle=(
                 I18n.Expl.confirm_operation,
                 I18n.Expl.hardware_wallet_confirm_operation,
               )
               operation={Operation.transaction(transfer)}
               sendOperation={sendTransfer(~transfer)}
               loading
             />
           }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>;
};
