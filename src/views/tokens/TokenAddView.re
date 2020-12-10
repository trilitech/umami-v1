open ReactNative;

module StateLenses = [%lenses
  type state = {
    name: string,
    address: string,
    symbol: string,
  }
];
module TokenCreateForm = ReForm.Make(StateLenses);

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
let make = (~cancel) => {
  let (tokenCreateRequest, createToken) = StoreContext.Tokens.useCreate();
  let addToast = LogsContext.useToast();

  let form: TokenCreateForm.api =
    TokenCreateForm.use(
      ~schema={
        TokenCreateForm.Validation.(
          Schema(nonEmpty(Name) + nonEmpty(Address) + nonEmpty(Symbol))
        );
      },
      ~onSubmit=
        ({state}) => {
          cancel();
          createToken((
            state.values.address,
            state.values.name,
            state.values.symbol,
          ))
          ->ApiRequest.logOk(addToast, Logs.Tokens, _ => I18n.t#token_created)
          ->ignore;

          None;
        },
      ~initialState={name: "", address: "", symbol: ""},
      (),
    );

  let onPressCancel = _ => cancel();

  let onSubmit = _ => {
    form.submit();
  };

  <ModalView.Form>
    {switch (tokenCreateRequest) {
     | Loading
     | Done(_) => React.null
     | NotAsked =>
       <>
         <Typography.Headline2 style=styles##title>
           I18n.title#add_token->React.string
         </Typography.Headline2>
         <FormGroupTextInput
           label=I18n.label#add_token_address
           value={form.values.address}
           handleChange={form.handleChange(Address)}
           error={form.getFieldError(Field(Address))}
           placeholder=I18n.input_placeholder#add_token_address
         />
         <FormGroupTextInput
           label=I18n.label#add_token_name
           value={form.values.name}
           handleChange={form.handleChange(Name)}
           error={form.getFieldError(Field(Name))}
           placeholder=I18n.input_placeholder#add_token_name
         />
         <FormGroupTextInput
           label=I18n.label#add_token_symbol
           value={form.values.symbol}
           handleChange={form.handleChange(Symbol)}
           error={form.getFieldError(Field(Symbol))}
           placeholder=I18n.input_placeholder#add_token_symbol
         />
         <View style=styles##formAction>
           <FormButton text=I18n.btn#cancel onPress=onPressCancel />
           <FormButton text=I18n.btn#register onPress=onSubmit />
         </View>
       </>
     }}
  </ModalView.Form>;
};
