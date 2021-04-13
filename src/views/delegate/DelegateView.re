open ReactNative;
open Delegate;

let styles =
  Style.(
    StyleSheet.create({
      "advancedOptionButton":
        style(
          ~flexDirection=`row,
          ~justifyContent=`spaceBetween,
          ~alignItems=`center,
          ~paddingVertical=8.->dp,
          ~marginVertical=10.->dp,
          ~paddingRight=12.->dp,
          (),
        ),
      "switchCmp": style(~height=16.->dp, ~width=32.->dp, ()),
      "switchThumb": style(~transform=[|scale(~scale=0.65)|], ()),
      "operationSummary": style(~marginBottom=20.->dp, ()),
      "deleteLoading": style(~paddingVertical=170.->dp, ()),
    })
  );

let buildTransaction = (state: DelegateForm.state, advancedOptionOpened) => {
  let mapIfAdvanced = (v, flatMap) =>
    advancedOptionOpened && v->Js.String2.length > 0 ? v->flatMap : None;

  Protocol.makeDelegate(
    ~source=state.values.sender,
    ~delegate=Some(state.values.baker),
    ~fee=?state.values.fee->mapIfAdvanced(ProtocolXTZ.fromString),
    ~forceLowFee=?
      advancedOptionOpened && state.values.forceLowFee ? Some(true) : None,
    (),
  );
};

type step =
  | SendStep
  | PasswordStep(Protocol.delegation, Protocol.simulationResults)
  | SubmittedStep(string);

let stepToString = step =>
  switch (step) {
  | SendStep => "sendstep"
  | PasswordStep(_, _) => "passwordstep"
  | SubmittedStep(_) => "submittedstep"
  };

module Form = {
  let build = (action: action, advancedOptionOpened, onSubmit) => {
    let (initAccount, initDelegate) =
      switch (action) {
      | Create(account) => (account, None)
      | Edit(account, _) => (Some(account), None)
      | Delete(account, delegate) => (Some(account), Some(delegate))
      };

    DelegateForm.use(
      ~schema={
        DelegateForm.Validation.(
          Schema(
            nonEmpty(Sender)
            + nonEmpty(Baker)
            + custom(values => FormUtils.isValidFloat(values.fee), Fee)
            + custom(
                values =>
                  switch (initDelegate) {
                  | Some(initDelegate) =>
                    initDelegate == values.baker
                      ? Error(I18n.form_input_error#change_baker) : Valid
                  | None => Valid
                  },
                Baker,
              ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          let operation = buildTransaction(state, advancedOptionOpened);
          onSubmit(operation);

          None;
        },
      ~initialState={
        sender: initAccount->Option.mapWithDefault("", a => a.address),
        baker: initDelegate->Option.getWithDefault(""),
        fee: "",
        forceLowFee: false,
      },
      ~i18n=FormUtils.i18n,
      (),
    );
  };

  module View = {
    open DelegateForm;

    let onAppear = (el, _) => {
      ReactFlipToolkit.spring({
        onUpdate: value => {
          el->ReactDOMRe.domElementToObj##style##opacity #= value;
        },
        delay: 50.,
        onComplete: () => (),
      });
    };

    let onExit = (el, _, removeElement) => {
      ReactFlipToolkit.spring({
        onUpdate: value => {
          el->ReactDOMRe.domElementToObj##style##opacity
          #= Js.Math.max_float(0., 1. -. value -. 0.1);
        },
        delay: 0.,
        onComplete: removeElement,
      });
    };

    [@react.component]
    let make = (~title, ~advancedOptionState, ~form, ~action, ~loading) => {
      let onSubmitDelegateForm = _ => {
        form.submit();
      };
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;

      let formFieldsAreValids =
        FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

      let initDelegate =
        switch (action) {
        | Create(_) => None
        | Edit(_, delegate) => Some(delegate)
        | Delete(_, delegate) => Some(delegate)
        };

      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <Typography.Headline style=FormStyles.header>
            title->React.string
          </Typography.Headline>
          <FormGroupDelegateSelector
            label=I18n.label#account_delegate
            value={form.values.sender}
            handleChange={form.handleChange(Sender)}
            error={form.getFieldError(Field(Sender))}
            disabled={
              switch (action) {
              | Create(None) => false
              | Create(Some(_))
              | Edit(_)
              | Delete(_) => true
              }
            }
          />
          <FormGroupBakerSelector
            label=I18n.label#baker
            value={form.values.baker}
            handleChange={form.handleChange(Baker)}
            filterOut=?initDelegate
            error={form.getFieldError(Field(Baker))}
          />
          <TouchableOpacity
            style=styles##advancedOptionButton
            activeOpacity=1.
            onPress={_ => setAdvancedOptionOpened(prev => !prev)}>
            <Typography.Overline2>
              I18n.btn#advanced_options->React.string
            </Typography.Overline2>
            <ThemedSwitch value=advancedOptionOpened />
          </TouchableOpacity>
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
                   <DelegateViewAdvancedOptions form />
                 </ReactFlipToolkit.FlippedView>
               : React.null}
          </ReactFlipToolkit.Flipper>
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView flipId="submit">
          <View style=FormStyles.verticalFormAction>
            <Buttons.SubmitPrimary
              text={
                switch (action) {
                | Create(_) => I18n.btn#delegation_submit
                | Edit(_) => I18n.btn#update
                | Delete(_) => I18n.btn#confirm
                }
              }
              onPress=onSubmitDelegateForm
              loading
              disabledLook={!formFieldsAreValids}
            />
          </View>
        </ReactFlipToolkit.FlippedView>
      </>;
    };
  };
};

let buildSummaryContent = (dryRun: Protocol.simulationResults) => {
  let revealFee =
    dryRun.revealFee != ProtocolXTZ.zero
      ? (
          I18n.label#implicit_reveal_fee,
          I18n.t#xtz_amount(dryRun.revealFee->ProtocolXTZ.toString),
        )
        ->Some
      : None;

  let fee = (
    I18n.label#fee,
    I18n.t#xtz_amount(
      ProtocolXTZ.Infix.(dryRun.fee - dryRun.revealFee)->ProtocolXTZ.toString,
    ),
  );

  let total = (
    I18n.label#summary_total,
    I18n.t#xtz_amount(dryRun.fee->ProtocolXTZ.toString),
  );

  [fee, ...revealFee->Option.mapWithDefault([total], r => [r, total])];
};

[@react.component]
let make = (~closeAction, ~action) => {
  let (advancedOptionOpened, _) as advancedOptionState =
    React.useState(_ => false);

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendOperation = (delegation, password) =>
    sendOperation(OperationApiRequest.delegate(delegation, password))
    ->Future.tapOk(hash => {setModalStep(_ => SubmittedStep(hash))})
    ->ignore;

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  React.useEffect0(() => {
    switch (action) {
    | Delete(account, _) =>
      let op =
        Protocol.makeDelegate(~source=account.address, ~delegate=None, ());
      sendOperationSimulate(op->Operation.Simulation.delegation)
      ->Future.tapOk(dryRun => {setModalStep(_ => PasswordStep(op, dryRun))})
      ->ignore;

    | _ => ()
    };

    None;
  });

  let form =
    Form.build(action, advancedOptionOpened, op =>
      sendOperationSimulate(op->Operation.Simulation.delegation)
      ->Future.tapOk(dryRun => {setModalStep(_ => PasswordStep(op, dryRun))})
      ->ignore
    );

  let title =
    switch (action) {
    | Create(_) => I18n.title#delegate
    | Edit(_) => I18n.title#delegate_update
    | Delete(_) => I18n.title#delegate_delete
    };

  let closing = ModalFormView.Close(closeAction);

  let back =
    switch (modalStep, action) {
    | (PasswordStep(_, _), Create(_))
    | (PasswordStep(_, _), Edit(_)) =>
      Some(() => setModalStep(_ => SendStep))
    | _ => None
    };

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading;
  let loading = operationRequest->ApiRequest.isLoading;

  let onPressCancel = _ => closeAction();

  <ReactFlipToolkit.Flipper
    flipKey={advancedOptionOpened->string_of_bool ++ modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView back closing>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch (modalStep) {
           | SubmittedStep(hash) => <SubmittedView hash onPressCancel />
           | SendStep =>
             switch (action) {
             | Delete(_) =>
               <View>
                 <View style=FormStyles.header>
                   <Typography.Headline>
                     I18n.title#delegate_delete->React.string
                   </Typography.Headline>
                 </View>
                 <LoadingView style=styles##deleteLoading />
               </View>
             | _ =>
               <Form.View
                 title
                 advancedOptionState
                 form
                 action
                 loading=loadingSimulate
               />
             }
           | PasswordStep(delegation, dryRun) =>
             let (target, title) =
               switch (delegation.delegate) {
               | None => (
                   ("", I18n.title#withdraw_baker),
                   I18n.title#delegate_delete,
                 )
               | Some(d) => (
                   (d, I18n.title#baker_account),
                   I18n.title#confirm_delegate,
                 )
               };
             <SignOperationView
               source=(delegation.source, I18n.title#delegated_account)
               destinations={`One(target)}
               title
               subtitle=I18n.expl#confirm_operation
               showCurrency=I18n.t#xtz_amount
               content={buildSummaryContent(dryRun)}
               sendOperation={sendOperation(delegation)}
               loading
             />;
           }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>;
};
