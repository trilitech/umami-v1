/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

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
      "overline": style(~marginBottom=2.->dp, ~textAlign=`center, ()),
    })
  );

[@react.component]
let make = (~chain, ~closeAction) => {
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
          state.values.address
          ->PublicKeyHash.build
          ->Future.value
          ->Future.flatMapOk(address =>
              checkToken(address)->Future.mapOk(res => (address, res))
            )
          ->Future.get(result =>
              switch (result) {
              | Ok((address, true)) =>
                createToken({
                  address,
                  alias: state.values.name,
                  symbol: state.values.symbol,
                  chain,
                })
                ->ApiRequest.logOk(addToast, Logs.Tokens, _ =>
                    I18n.t#token_created
                  )
                ->FutureEx.getOk(_ => closeAction())
              | Error(_)
              | Ok((_, false)) =>
                let errorMsg = I18n.t#error_check_contract;
                addToast(Logs.log(~kind=Error, ~origin=Tokens, errorMsg));
                raiseSubmitFailed(Some(errorMsg));
              }
            );

          None;
        },
      ~initialState={name: "", address: "", symbol: ""},
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading = tokenCreateRequest->ApiRequest.isLoading;

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

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
      clearButton=true
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
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>;
};
