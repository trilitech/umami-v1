open ReactNative;

module StateLenses = [%lenses type state = {name: string}];
module AccountCreateForm = ReForm.Make(StateLenses);

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
let make = (~cancel, ~handleAdd) => {
  let (accountRequest, createAccount) = AccountApiRequest.useCreate();
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
          ->Future.tapOk(_ => {handleAdd()})
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
     | Loading
     | Done(_) => <> </>
     | NotAsked =>
       <>
         <Typography.Headline2 style=styles##title>
           I18n.title#account_create->React.string
         </Typography.Headline2>
         <FormGroupTextInput
           label=I18n.label#account_create_name
           value={form.values.name}
           handleChange={form.handleChange(Name)}
           error={form.getFieldError(Field(Name))}
         />
         <View style=styles##formAction>
           <FormButton text=I18n.btn#cancel onPress=onPressCancel />
           <FormButton text=I18n.btn#create onPress=onSubmit />
         </View>
       </>
     }}
  </ModalView.Form>;
};
