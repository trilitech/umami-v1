open ReactNative;

module StateLenses = [%lenses type state = {name: string}];
module AccountCreateForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "loadingView":
        style(
          ~height=400.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~cancel) => {
  let (accountRequest, createAccount) = StoreContext.Accounts.useCreate();
  let addLog = LogsContext.useAdd();

  let form: AccountCreateForm.api =
    AccountCreateForm.use(
      ~schema={
        AccountCreateForm.Validation.(Schema(nonEmpty(Name)));
      },
      ~onSubmit=
        ({state}) => {
          cancel();
          createAccount(state.values.name)
          ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
              I18n.t#account_created
            )
          ->ignore;
          None;
        },
      ~initialState={name: ""},
      (),
    );

  let onPressCancel = _ => cancel();

  let onSubmit = _ => {
    form.submit();
  };

  <ModalView.Form>
    {switch (accountRequest) {
     | Loading(_)
     | Done(_) => <> </>
     | NotAsked =>
       <>
         <Typography.Headline style=FormStyles.title>
           I18n.title#account_create->React.string
         </Typography.Headline>
         <FormGroupTextInput
           label=I18n.label#account_create_name
           value={form.values.name}
           handleChange={form.handleChange(Name)}
           error={form.getFieldError(Field(Name))}
         />
         <View style=FormStyles.formAction>
           <Buttons.FormPrimary text=I18n.btn#cancel onPress=onPressCancel />
           <Buttons.FormPrimary text=I18n.btn#create onPress=onSubmit />
         </View>
       </>
     }}
  </ModalView.Form>;
};
