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
let make = (~derivationScheme, ~setDerivationScheme, ~onPressCancel, ~goNextStep) => {
  let defaultDerivationScheme = "m/44'/1729'/?'/0'";

  let form: SelectDerivationPathForm.api =
    SelectDerivationPathForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        SelectDerivationPathForm.Validation.(
          Schema(nonEmpty(SelectedDerivationScheme))
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
        hasError=false
        placeholder=I18n.input_placeholder#enter_derivation_path
      />
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
