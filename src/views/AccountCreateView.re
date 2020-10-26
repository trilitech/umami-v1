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

  <ModalView>
    {switch (accountRequest) {
     | Done(Ok(_result)) =>
       <>
         <Typography.H1 style=styles##title>
           "Account created"->React.string
         </Typography.H1>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | Done(Error(error)) =>
       <>
         <Typography.H1 style=FormLabel.styles##label>
           error->React.string
         </Typography.H1>
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
         <Typography.H1 style=styles##title>
           "Create new account"->React.string
         </Typography.H1>
         <FormGroupTextInput
           label="Name"
           value={form.values.name}
           handleChange={form.handleChange(Name)}
           error={form.getFieldError(Field(Name))}
         />
         <View style=styles##formAction>
           <FormButton text="CANCEL" onPress=onPressCancel />
           <FormButton
             text="CREATE"
             onPress=onSubmit
             disabled={form.formState == Errored}
           />
         </View>
       </>
     }}
  </ModalView>;
};
