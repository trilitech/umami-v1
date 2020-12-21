open ReactNative;
open Delegate;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=20.->dp, ~textAlign=`center, ()),
      "formAction":
        style(
          ~flexDirection=`row,
          ~justifyContent=`center,
          ~marginTop=24.->dp,
          (),
        ),
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
      "loadingView":
        style(
          ~height=400.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

let buildTransaction = (state: DelegateForm.state, advancedOptionOpened) => {
  let mapIfAdvanced = (v, map) =>
    advancedOptionOpened && v->Js.String2.length > 0 ? Some(v->map) : None;

  Injection.makeDelegate(
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
  | PasswordStep(Injection.operation);

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
    let make = (~title, ~onPressCancel, ~advancedOptionState, ~form, ~action) => {
      let onSubmitDelegateForm = _ => {
        form.submit();
      };
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;

      <>
        <Typography.Headline style=styles##title>
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
        <View style=styles##formAction>
          <FormButton text=I18n.btn#cancel onPress=onPressCancel />
          <FormButton
            text={
              switch (action) {
              | Create(_) => I18n.btn#ok
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

[@react.component]
let make = (~onPressCancel, ~action) => {
  let (advancedOptionOpened, _) as advancedOptionState =
    React.useState(_ => false);

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendOperation = (operation, ~password) =>
    switch (operation) {
    | SendForm.InjectionOperation(operation) =>
      sendOperation(OperationApiRequest.{operation, password})->ignore
    | _ => ()
    };

  let (modalStep, setModalStep) =
    React.useState(_ =>
      switch (action) {
      | Create(_)
      | Edit(_) => SendStep
      | Delete(account, _delegate) =>
        PasswordStep(
          Injection.makeDelegate(~source=account.address, ~delegate="", ()),
        )
      }
    );

  let form =
    Form.build(action, advancedOptionOpened, op =>
      setModalStep(_ => PasswordStep(op))
    );

  let title =
    switch (action) {
    | Create(_) => I18n.title#delegate
    | Edit(_) => I18n.title#delegate_update
    | Delete(_) => I18n.title#delegate_delete
    };

  let theme = ThemeContext.useTheme();

  <ModalView.Form>
    {switch (modalStep, operationRequest) {
     | (_, Done(Ok((hash, _)), _)) =>
       <>
         <Typography.Headline style=styles##title>
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
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, Done(Error(error), _)) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, Loading(_)) =>
       <View style=styles##loadingView>
         <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.large
           color={theme.colors.iconMediumEmphasis}
         />
       </View>
     | (SendStep, _) =>
       <Form.View title onPressCancel advancedOptionState form action />
     | (PasswordStep(operation), _) =>
       <SignOperationView
         onPressCancel={event =>
           switch (action) {
           | Create(_)
           | Edit(_) => setModalStep(_ => SendStep)
           | Delete(_) => onPressCancel(event)
           }
         }
         title
         operation={SendForm.InjectionOperation(operation)}
         sendOperation
       />
     }}
  </ModalView.Form>;
};
