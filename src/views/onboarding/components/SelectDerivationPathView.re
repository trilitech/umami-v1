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
    selectedDerivationPath: string,
    customDerivationPath: string,
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

let form = (~custom, ~setDerivationPath, ~next, ~derivationPath) =>
  SelectDerivationPathForm.use(
    ~validationStrategy=OnDemand,
    ~schema={
      SelectDerivationPathForm.Validation.(
        Schema(
          custom(
            values =>
              if (values.selectedDerivationPath
                  == DerivationPath.Pattern.defaultString) {
                Valid;
              } else {
                values.customDerivationPath->FormUtils.checkDerivationPath;
              },
            CustomDerivationPath,
          )
          + nonEmpty(SelectedDerivationPath),
        )
      );
    },
    ~onSubmit=
      ({state}) => {
        setDerivationPath(_
          // Errors should be filtered by form errors hence the getExn
          =>
            state.values.selectedDerivationPath
            ->DerivationPath.Pattern.fromString
            ->Result.getExn
          );
        next(state);
        None;
      },
    ~initialState={
      selectedDerivationPath: derivationPath->DerivationPath.Pattern.toString,
      customDerivationPath:
        derivationPath->DerivationPath.Pattern.isDefault && !custom
          ? "" : derivationPath->DerivationPath.Pattern.toString,
    },
    ~i18n=FormUtils.i18n,
    (),
  );

module DerivationPathInput = {
  [@react.component]
  let make = (~form: SelectDerivationPathForm.api, ~onSubmit) => {
    let error = {
      form.getFieldError(Field(CustomDerivationPath));
    };

    <>
      <ThemedTextInput
        value={form.values.customDerivationPath}
        onFocus={_ =>
          form.handleChange(
            SelectedDerivationPath,
            form.values.customDerivationPath,
          )
        }
        onValueChange={value => {
          form.handleChange(CustomDerivationPath, value);
          form.handleChange(SelectedDerivationPath, value);
        }}
        hasError={error->Option.isSome}
        placeholder=I18n.Input_placeholder.enter_derivation_path
        onSubmitEditing=onSubmit
      />
      <FormError ?error />
    </>;
  };
};

[@react.component]
let make = (~derivationPath, ~setDerivationPath, ~goNextStep) => {
  let form: SelectDerivationPathForm.api =
    form(
      ~custom=false,
      ~derivationPath,
      ~setDerivationPath,
      ~next=goNextStep,
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <>
    <FormGroup style=Style.(arrayOption([|Some(styles##formGroup)|]))>
      <RadioItem
        label=I18n.Label.account_default_path
        value=DerivationPath.Pattern.defaultString
        setValue={value =>
          form.handleChange(SelectedDerivationPath, value(""))
        }
        currentValue={form.values.selectedDerivationPath}
      />
      <RadioItem
        label=I18n.Label.account_custom_path
        value={form.values.customDerivationPath}
        setValue={value =>
          form.handleChange(SelectedDerivationPath, value(""))
        }
        currentValue={form.values.selectedDerivationPath}
      />
      <DerivationPathInput form onSubmit />
    </FormGroup>
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitPrimary
        text=I18n.Btn.continue
        onPress=onSubmit
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </>;
};
