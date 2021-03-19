open ReactNative;
open SettingsComponents;

module StateLenses = [%lenses type state = {confirmations: string}];
module VerificationForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "row": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
      "spacerForInputError": style(~marginBottom=20.->dp, ()),
      "button": style(~width=104.->dp, ~height=34.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  let writeConf = ConfigContext.useWrite();
  let settings = SdkContext.useSettings();
  let addToast = LogsContext.useToast();

  let form: VerificationForm.api =
    VerificationForm.use(
      ~schema={
        VerificationForm.Validation.(
          Schema(
            nonEmpty(Confirmations)
            + custom(
                values => FormUtils.isValidInt(values.confirmations),
                Confirmations,
              ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          let confirmations = state.values.confirmations->int_of_string;

          writeConf(c =>
            {
              ...c,
              confirmations:
                confirmations != ConfigFile.Default.confirmations
                  ? Some(confirmations) : None,
            }
          );
          addToast(
            Logs.info(~origin=Settings, I18n.settings#confirmations_saved),
          );

          None;
        },
      ~initialState={
        confirmations:
          settings.config.confirmations
          ->Option.mapWithDefault("", Int.toString),
      },
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <Block title=I18n.settings#confirmations_title>
    <View accessibilityRole=`form style=styles##row>
      <ColumnLeft style=styles##spacerForInputError>
        <Typography.Body1>
          I18n.settings#confirmations_label->React.string
        </Typography.Body1>
      </ColumnLeft>
      <ColumnRight>
        <SettingTextInput
          value={form.values.confirmations}
          onValueChange={form.handleChange(Confirmations)}
          error={form.getFieldError(Field(Confirmations))}
          placeholder={ConfigFile.Default.confirmations->Int.toString}
          keyboardType=`numeric
          onSubmitEditing=onSubmit
        />
      </ColumnRight>
      <ColumnRight style=styles##spacerForInputError>
        <Buttons.SubmitPrimary
          style=styles##button
          text=I18n.btn#save
          onPress=onSubmit
          disabledLook={!formFieldsAreValids}
        />
      </ColumnRight>
    </View>
  </Block>;
};
