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
    })
  );

let buildTransaction = (state: DelegateForm.state, advancedOptionOpened) => {
  let mapIfAdvanced = (v, map) =>
    advancedOptionOpened && v->Js.String2.length > 0 ? Some(v->map) : None;

  Protocol.makeDelegate(
    ~source=state.values.sender,
    ~delegate=state.values.baker,
    ~fee=?state.values.fee->mapIfAdvanced(Js.Float.fromString),
    ~forceLowFee=?
      advancedOptionOpened && state.values.forceLowFee ? Some(true) : None,
    (),
  );
};

type step =
  | SendStep
  | PasswordStep(Protocol.delegation, Protocol.simulationResults);

module Form = {
  let build = (action: action, advancedOptionOpened, onSubmit) => {
    let (initAccount, initDelegate) =
      switch (action) {
      | Create(account) => (account, None)
      | Edit(account, delegate)
      | Delete(account, delegate) => (Some(account), Some(delegate))
      };

    DelegateForm.use(
      ~validationStrategy=OnDemand,
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
                      ? Error("not the same baker") : Valid
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
        sender: initAccount->Belt.Option.mapWithDefault("", a => a.address),
        baker: initDelegate->Belt.Option.getWithDefault(""),
        fee: "",
        forceLowFee: false,
      },
      (),
    );
  };

  module View = {
    open DelegateForm;

    [@react.component]
    let make = (~title, ~advancedOptionState, ~form, ~action) => {
      let onSubmitDelegateForm = _ => {
        form.submit();
      };
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;

      <>
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
          error={form.getFieldError(Field(Baker))}
        />
        <View>
          <TouchableOpacity
            style=styles##advancedOptionButton
            activeOpacity=1.
            onPress={_ => setAdvancedOptionOpened(prev => !prev)}>
            <Typography.Overline2>
              I18n.btn#advanced_options->React.string
            </Typography.Overline2>
            <ThemedSwitch value=advancedOptionOpened />
          </TouchableOpacity>
          {advancedOptionOpened
             ? <DelegateViewAdvancedOptions form /> : React.null}
        </View>
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
          />
        </View>
      </>;
    };
  };
};

let buildSummaryContent = (dryRun: Protocol.simulationResults) => [
  (I18n.label#fee, I18n.t#xtz_amount(dryRun.fee->Js.Float.toString)),
];

[@react.component]
let make = (~onPressCancel, ~action) => {
  let (advancedOptionOpened, _) as advancedOptionState =
    React.useState(_ => false);

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendOperation = (delegation, password) =>
    sendOperation(OperationApiRequest.delegate(delegation, password))
    ->ignore;

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  React.useEffect0(() => {
    switch (action) {
    | Delete(account, _) =>
      let op =
        Protocol.makeDelegate(~source=account.address, ~delegate="", ());
      sendOperationSimulate(op->Operation.delegation)
      ->Future.tapOk(dryRun => {setModalStep(_ => PasswordStep(op, dryRun))})
      ->ignore;

    | _ => ()
    };

    None;
  });

  let form =
    Form.build(action, advancedOptionOpened, op =>
      sendOperationSimulate(op->Operation.delegation)
      ->Future.tapOk(dryRun => {setModalStep(_ => PasswordStep(op, dryRun))})
      ->ignore
    );

  let title =
    switch (action) {
    | Create(_) => I18n.title#delegate
    | Edit(_) => I18n.title#delegate_update
    | Delete(_) => I18n.title#delegate_delete
    };

  let closing = ModalView.Close(_ => onPressCancel());
  let onPressCancel = _ => onPressCancel();

  <ModalView.Form closing>
    {switch (modalStep, operationRequest, operationSimulateRequest) {
     | (_, Done(Ok((hash, _)), _), _) =>
       <>
         <Typography.Headline style=FormStyles.header>
           {switch (action) {
            | Create(_) => I18n.title#delegation_sent
            | Edit(_) => I18n.title#baker_updated
            | Delete(_) => I18n.title#delegation_deleted
            }}
           ->React.string
         </Typography.Headline>
         <Typography.Overline2>
           I18n.t#operation_hash->React.string
         </Typography.Overline2>
         <Typography.Body1> hash->React.string </Typography.Body1>
         <View style=FormStyles.formAction>
           <Buttons.FormPrimary text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, Done(Error(error), _), _) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=FormStyles.formAction>
           <Buttons.FormPrimary text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, _, Loading(_)) =>
       <ModalView.LoadingView title=I18n.title#simulation />
     | (_, Loading(_), _) =>
       <ModalView.LoadingView title=I18n.title#submitting />
     | (SendStep, _, _) => <Form.View title advancedOptionState form action />
     | (PasswordStep(delegation, dryRun), _, _) =>
       <SignOperationView
         back={() =>
           switch (action) {
           | Create(_)
           | Edit(_) => setModalStep(_ => SendStep)
           | Delete(_) => onPressCancel()
           }
         }
         source=(delegation.source, I18n.title#delegated_account)
         destinations={`One((delegation.delegate, I18n.title#baker_account))}
         title=I18n.title#confirm_delegate
         subtitle=I18n.expl#confirm_operation
         content={buildSummaryContent(dryRun)}
         sendOperation={sendOperation(delegation)}
       />
     }}
  </ModalView.Form>;
};
