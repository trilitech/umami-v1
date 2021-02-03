open ReactNative;
open SettingsComponents;

module StateLenses = [%lenses type state = {confirmations: string}];
module VerificationForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "row":
        style(
          ~flex=1.,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~marginBottom=20.->dp,
          (),
        ),
      "button": style(~width=104.->dp, ~height=34.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  let writeConf = ConfigContext.useWrite();
  let settings = ConfigContext.useSettings();
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
          writeConf(c =>
            {
              ...c,
              confirmations: Some(state.values.confirmations->int_of_string),
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
          ->Option.getWithDefault(ConfigFile.confirmations)
          ->string_of_int,
      },
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <Block title=I18n.settings#confirmations_title>
    <View accessibilityRole=`form style=styles##row>
      <ColumnLeft>
        <Typography.Body1>
          I18n.settings#confirmations_label->React.string
        </Typography.Body1>
      </ColumnLeft>
      <ColumnRight>
        <SettingTextInput
          value={form.values.confirmations}
          onValueChange={form.handleChange(Confirmations)}
          error={form.getFieldError(Field(Confirmations))}
          keyboardType=`numeric
          onSubmitEditing=onSubmit
        />
      </ColumnRight>
      <ColumnRight>
        <Buttons.SubmitPrimary
          style=styles##button
          text=I18n.btn#save
          onPress=onSubmit
        />
      </ColumnRight>
    </View>
  </Block>;
};
