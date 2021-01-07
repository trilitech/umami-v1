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
      "title": style(~marginBottom=6.->dp, ~textAlign=`center, ()),
      "overline": style(~marginBottom=20.->dp, ~textAlign=`center, ()),
    })
  );

[@react.component]
let make = (~closeAction) => {
  let (tokenCreateRequest, createToken) = StoreContext.Tokens.useCreate();
  let (_checkTokenRequest, checkToken) = StoreContext.Tokens.useCheck();
  let addToast = LogsContext.useToast();

  let form: TokenCreateForm.api =
    TokenCreateForm.use(
      ~schema={
        TokenCreateForm.Validation.(
          Schema(nonEmpty(Name) + nonEmpty(Address) + nonEmpty(Symbol))
        );
      },
      ~onSubmit=
        ({state, raiseSubmitFailed}) => {
          checkToken(state.values.address)
          ->Future.get(result =>
              switch (result) {
              | Ok(_) =>
                createToken({
                  address: state.values.address,
                  alias: state.values.name,
                  symbol: state.values.symbol,
                })
                ->Future.tapOk(_ => closeAction())
                ->ApiRequest.logOk(addToast, Logs.Tokens, _ =>
                    I18n.t#token_created
                  )
                ->ignore
              | Error(_) =>
                let errorMsg = I18n.t#error_check_contract;
                addToast(Logs.error(~origin=Tokens, errorMsg));
                raiseSubmitFailed(Some(errorMsg));
              }
            );

          None;
        },
      ~initialState={name: "", address: "", symbol: ""},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading = tokenCreateRequest != ApiRequest.NotAsked;

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=styles##title>
      I18n.title#add_token->React.string
    </Typography.Headline>
    <Typography.Overline3 style=styles##overline>
      I18n.t#add_token_format_contract_sentence->React.string
    </Typography.Overline3>
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
    <Buttons.SubmitPrimary
      text=I18n.btn#register
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
    />
  </ModalFormView>;
};
