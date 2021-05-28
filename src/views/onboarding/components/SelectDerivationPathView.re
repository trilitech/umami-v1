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
    selectedDerivationScheme: string,
    customDerivationScheme: string,
  }
];

module SelectDerivationPathForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~marginBottom=0.->dp, ()),
      "label": style(~marginVertical=4.->dp, ()),
      "decoration":
        style(
          ~display=`flex,
          ~alignItems=`center,
          ~position=`absolute,
          ~marginTop=auto,
          ~marginBottom=auto,
          ~top=0.->dp,
          ~bottom=0.->dp,
          ~right=10.->dp,
          (),
        ),
    })
  );

[@react.component]
let make =
    (~derivationScheme, ~setDerivationScheme, ~onPressCancel, ~goNextStep) => {
  let defaultDerivationScheme = "m/44'/1729'/?'/0'";

  let form: SelectDerivationPathForm.api =
    SelectDerivationPathForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        SelectDerivationPathForm.Validation.(
          Schema(
            custom(
              values =>
                if (values.selectedDerivationScheme == defaultDerivationScheme) {
                  Valid;
                } else if (Js.Re.test_(
                             Js.Re.fromString("^m(/[0-9?]+')+$"),
                             values.customDerivationScheme,
                           )) {
                  Valid;
                } else {
                  Error(I18n.form_input_error#derivation_path_error);
                },
              CustomDerivationScheme,
            )
            + nonEmpty(SelectedDerivationScheme),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          setDerivationScheme(_ => state.values.selectedDerivationScheme);
          goNextStep();
          None;
        },
      ~initialState={
        selectedDerivationScheme: derivationScheme,
        customDerivationScheme:
          derivationScheme == defaultDerivationScheme ? "" : derivationScheme,
      },
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  let error = {
    form.getFieldError(Field(CustomDerivationScheme));
  };

  <>
    <FormGroup style=Style.(arrayOption([|Some(styles##formGroup)|]))>
      <RadioItem
        label=I18n.label#account_default_path
        value=defaultDerivationScheme
        setValue={value =>
          form.handleChange(SelectedDerivationScheme, value(""))
        }
        currentValue={form.values.selectedDerivationScheme}
      />
      <RadioItem
        label=I18n.label#account_custom_path
        value={form.values.customDerivationScheme}
        setValue={value =>
          form.handleChange(SelectedDerivationScheme, value(""))
        }
        currentValue={form.values.selectedDerivationScheme}
      />
      <ThemedTextInput
        value={form.values.customDerivationScheme}
        onFocus={_ =>
          form.handleChange(
            SelectedDerivationScheme,
            form.values.customDerivationScheme,
          )
        }
        onValueChange={value => {
          form.handleChange(CustomDerivationScheme, value);
          form.handleChange(SelectedDerivationScheme, value);
        }}
        hasError={error->Option.isSome}
        placeholder=I18n.input_placeholder#enter_derivation_path
      />
      <FormError ?error />
    </FormGroup>
    <View style=FormStyles.formActionSpaceBetween>
      <Buttons.Form text=I18n.btn#back onPress=onPressCancel />
      <Buttons.SubmitPrimary
        text=I18n.btn#continue
        onPress=onSubmit
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </>;
};
