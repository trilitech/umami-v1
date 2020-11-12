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
let make = (~onPressCancel) => {
  let (accountRequest, createAccount) = AccountApiRequest.useCreateAccount();

  let form: AccountCreateForm.api =
    AccountCreateForm.use(
      ~schema={
        AccountCreateForm.Validation.(Schema(nonEmpty(Name)));
      },
      ~onSubmit=
        ({state}) => {
          createAccount(state.values.name);
          None;
        },
      ~initialState={name: ""},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <ModalView.Form>
    {switch (accountRequest) {
     | Done(Ok(_result)) =>
       <>
         <Typography.Headline2 style=styles##title>
           "Account created"->React.string
         </Typography.Headline2>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | Done(Error(error)) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | Loading =>
       <View style=styles##loadingView>
         <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.large
           color="#FFF"
         />
       </View>
     | NotAsked =>
       <>
         <Typography.Headline2 style=styles##title>
           "Create new account"->React.string
         </Typography.Headline2>
         <FormGroupTextInput
           label="Name"
           value={form.values.name}
           handleChange={form.handleChange(Name)}
           error={form.getFieldError(Field(Name))}
         />
         <View style=styles##formAction>
           <FormButton text="CANCEL" onPress=onPressCancel />
           <FormButton text="CREATE" onPress=onSubmit />
         </View>
       </>
     }}
  </ModalView.Form>;
};
