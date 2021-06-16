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

let styles =
  Style.(
    StyleSheet.create({"operationSummary": style(~marginBottom=20.->dp, ())})
  );

[@react.component]
let make =
    (
      ~title=?,
      ~subtitle=?,
      ~source,
      ~destinations,
      ~showCurrency,
      ~sendOperation: _ => Future.t(Result.t(_)),
      ~content: list((string, Belt.List.t(TezosClient.Transfer.currency))),
      ~loading=false,
    ) => {
  let (wrongPassword, setWrongPassword) = React.useState(() => false);

  let account =
    StoreContext.Accounts.useGetFromAddress(source->fst)->Option.getExn;

  let form: SendForm.Password.api =
    SendForm.Password.use(
      ~schema={
        SendForm.Password.Validation.(Schema(nonEmpty(Password)));
      },
      ~onSubmit=
        ({state, raiseSubmitFailed}) => {
          sendOperation(state.values.password)
          ->Future.tapError(
              fun
              | API.Error.Taquito(WrongPassword) =>
                raiseSubmitFailed(Some(I18n.form_input_error#wrong_password))
              | _ => (),
            )
          ->ignore;

          None;
        },
      ~initialState={password: ""},
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <>
    {title->ReactUtils.mapOpt(title =>
       <View style=FormStyles.header>
         <Typography.Headline> title->React.string </Typography.Headline>
         {subtitle->ReactUtils.mapOpt(subtitle =>
            <Typography.Overline1 style=FormStyles.subtitle>
              subtitle->React.string
            </Typography.Overline1>
          )}
       </View>
     )}
    <OperationSummaryView
      style=styles##operationSummary
      source=(account.address, source->snd)
      destinations
      showCurrency
      content
    />
    <FormGroupTextInput
      label=I18n.label#password
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={
        [
          form.formState->FormUtils.getFormStateError,
          form.getFieldError(Field(Password)),
        ]
        ->UmamiCommon.Lib.Option.firstSome
      }
      textContentType=`password
      secureTextEntry=true
      onSubmitEditing=onSubmit
    />
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitPrimary
        text=I18n.btn#confirm
        onPress=onSubmit
        loading
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </>;
};
