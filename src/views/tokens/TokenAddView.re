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
open Let;

module StateLenses = [%lenses
  type state = {
    name: string,
    address: string,
    symbol: string,
    decimals: string,
  }
];
module TokenCreateForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=6.->dp, ~textAlign=`center, ()),
      "overline": style(~marginBottom=24.->dp, ~textAlign=`center, ()),
    })
  );

module FormMetadata = {
  [@react.component]
  let make = (~form: TokenCreateForm.api) => {
    <>
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
      <FormGroupTextInput
        label=I18n.label#add_token_decimals
        value={form.values.decimals}
        handleChange={form.handleChange(Decimals)}
        error={form.getFieldError(Field(Name))}
        placeholder=I18n.input_placeholder#add_token_decimals
      />
    </>;
  };
};

module MetadataForm = {
  [@react.component]
  let make = (~form: TokenCreateForm.api, ~pkh) => {
    let metadata = MetadataApiRequest.useLoadMetadata(pkh);
    React.useEffect1(
      () => {
        switch (metadata) {
        | Loading(Some(metadata))
        | Done(Ok(metadata), _) =>
          form.handleChange(Name, metadata.name);
          form.handleChange(Symbol, metadata.symbol);
        | Done(Error(_), _) => ()
        | _ => ()
        };
        None;
      },
      [|metadata|],
    );
    switch (metadata) {
    | Done(Ok(_), _) => <FormMetadata form />
    | Done(Error(_err), _) => <FormMetadata form />
    | NotAsked => React.null
    | Loading(_) => <LoadingView />
    };
  };
};

[@react.component]
let make = (~chain, ~address="", ~closeAction) => {
  let (tokenCreateRequest, createToken) = StoreContext.Tokens.useCreate();

  let form: TokenCreateForm.api =
    TokenCreateForm.use(
      ~schema={
        TokenCreateForm.Validation.(
          Schema(
            nonEmpty(Name)
            + custom(
                state =>
                  switch (state.decimals->Int.fromString) {
                  | None => Error(I18n.form_input_error#not_an_int)
                  | Some(i) when i < 0 =>
                    Error(I18n.form_input_error#negative_int)
                  | Some(_) => Valid
                  },
                Decimals,
              )
            + custom(
                state =>
                  switch (PublicKeyHash.build(state.address)) {
                  | Error(_) => Error(I18n.form_input_error#invalid_key_hash)
                  | Ok(_) => Valid
                  },
                Address,
              )
            + nonEmpty(Symbol),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          FutureEx.async(() => {
            let%FResMap address =
              state.values.address->PublicKeyHash.build->Future.value;
            createToken({
              kind: FA1_2,
              address,
              alias: state.values.name,
              symbol: state.values.symbol,
              chain,
              decimals:
                state.values.decimals |> Int.fromString |> Option.getExn,
            })
            ->FutureEx.getOk(_ => closeAction());
          });

          None;
        },
      ~initialState={name: "", address, symbol: "", decimals: ""},
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading = tokenCreateRequest->ApiRequest.isLoading;

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  let pkh = PublicKeyHash.build(form.values.address);

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
    {switch (pkh) {
     | Ok(pkh) => <MetadataForm form pkh />
     | Error(_) => React.null
     }}
    <Buttons.SubmitPrimary
      text=I18n.btn#register
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>;
};
