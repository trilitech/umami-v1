open ReactNative;
open Common;

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
  let build =
      (
        initAccount: option(Account.t),
        initDelegate: option(string),
        advancedOptionOpened,
        onSubmit,
      ) => {
    DelegateForm.use(
      ~schema={
        DelegateForm.Validation.(
          Schema(
            nonEmpty(Sender)
            + nonEmpty(Baker)
            + custom(values => FormUtils.isValidFloat(values.fee), Fee),
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
    let make =
        (~onPressCancel, ~advancedOptionState, ~form, ~blockSender, ~hasBaker) => {
      let onSubmitDelegateForm = _ => {
        form.submit();
      };
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;

      <>
        <Typography.Headline2 style=styles##title>
          {hasBaker ? "Change Baker" : "Delegate"}->React.string
        </Typography.Headline2>
        <FormGroupDelegateSelector
          label="Account to delegate"
          value={form.values.sender}
          handleChange={form.handleChange(Sender)}
          error={form.getFieldError(Field(Sender))}
          disabled=blockSender
        />
        <FormGroupBakerSelector
          label="Baker"
          value={form.values.baker}
          handleChange={form.handleChange(Baker)}
          error={form.getFieldError(Field(Baker))}
        />
        <View>
          <TouchableOpacity
            style=styles##advancedOptionButton
            activeOpacity=1.
            onPress={_ => setAdvancedOptionOpened(prev => !prev)}>
            <Typography.Overline1>
              "Advanced options"->React.string
            </Typography.Overline1>
            <SwitchNative
              value=advancedOptionOpened
              //onValueChange=handleChange
              thumbColor="#000"
              trackColor={Switch.trackColor(
                ~_true="#FFF",
                ~_false="rgba(255,255,255,0.5)",
                (),
              )}
              style=styles##switchCmp
              thumbStyle=styles##switchThumb
            />
          </TouchableOpacity>
          {advancedOptionOpened
             ? <DelegateViewAdvancedOptions form /> : React.null}
        </View>
        <View style=styles##formAction>
          <FormButton text="CANCEL" onPress=onPressCancel />
          <FormButton
            text={hasBaker ? "UPDATE" : "OK"}
            onPress=onSubmitDelegateForm
          />
        </View>
      </>;
    };
  };
};

[@react.component]
let make = (~onPressCancel, ~defaultAccount=?, ~defaultDelegate=?) => {
  let account = StoreContext.useAccount();
  let network = StoreContext.useNetwork();

  let (advancedOptionOpened, _) as advancedOptionState =
    React.useState(_ => false);

  let (operationRequest, sendOperation) =
    OperationApiRequest.useCreate(network);

  let sendOperation = (operation, ~password) =>
    account->Lib.Option.iter(account =>
      sendOperation(OperationApiRequest.{operation, password})->ignore
    );

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let form =
    Form.build(defaultAccount, defaultDelegate, advancedOptionOpened, op =>
      setModalStep(_ => PasswordStep(op))
    );

  React.useEffect0(() => {None});

  <ModalView.Form>
    {switch (modalStep, operationRequest) {
     | (_, Done(Ok(hash))) =>
       <>
         <Typography.Headline2 style=styles##title>
           "Delegation sent"->React.string
         </Typography.Headline2>
         <Typography.Overline1>
           "Operation hash"->React.string
         </Typography.Overline1>
         <Typography.Body1> hash->React.string </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | (_, Done(Error(error))) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | (_, Loading) =>
       <View style=styles##loadingView>
         <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.large
           color=Colors.highIcon
         />
       </View>
     | (SendStep, _) =>
       <Form.View
         onPressCancel
         advancedOptionState
         form
         blockSender={defaultAccount->Belt.Option.isSome}
         hasBaker={defaultDelegate->Belt.Option.isSome}
       />
     | (PasswordStep(operation), _) =>
       <SignOperationView
         onPressCancel={_ => setModalStep(_ => SendStep)}
         operation
         sendOperation
       />
     }}
  </ModalView.Form>;
};
