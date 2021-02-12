open ReactNative;
open SettingsComponents;

module StateLenses = [%lenses
  type state = {
    endpointTest: string,
    explorerTest: string,
  }
];
module ChainForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "row": style(~flex=1., ~flexDirection=`row, ()),
      "button": style(~height=34.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  let writeConf = ConfigContext.useWrite();
  let settings = ConfigContext.useSettings();
  let addToast = LogsContext.useToast();

  let form: ChainForm.api =
    ChainForm.use(
      ~schema={
        ChainForm.Validation.(
          Schema(nonEmpty(EndpointTest) + nonEmpty(ExplorerTest))
        );
      },
      ~onSubmit=
        ({state}) => {
          writeConf(c =>
            {
              ...c,
              endpointTest:
                state.values.endpointTest->Js.String2.length > 0
                && state.values.endpointTest != ConfigFile.Default.endpointTest
                  ? Some(state.values.endpointTest) : None,
              explorerTest:
                state.values.explorerTest->Js.String2.length > 0
                && state.values.explorerTest != ConfigFile.Default.explorerTest
                  ? Some(state.values.explorerTest) : None,
            }
          );
          addToast(Logs.info(~origin=Settings, I18n.settings#chain_saved));

          None;
        },
      ~initialState={
        endpointTest: settings.config.endpointTest->Option.getWithDefault(""),
        explorerTest: settings.config.explorerTest->Option.getWithDefault(""),
      },
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <Block title=I18n.settings#chain_title>
    <View accessibilityRole=`form style=styles##row>
      <ColumnLeft>
        <Typography.Body1> I18n.t#testnet->React.string </Typography.Body1>
      </ColumnLeft>
      <ColumnRight>
        <SettingFormGroupTextInput
          label=I18n.settings#chain_node_label
          value={form.values.endpointTest}
          onValueChange={form.handleChange(EndpointTest)}
          error={form.getFieldError(Field(EndpointTest))}
          onSubmitEditing=onSubmit
        />
        <SettingFormGroupTextInput
          label=I18n.settings#chain_mezos_label
          value={form.values.explorerTest}
          onValueChange={form.handleChange(ExplorerTest)}
          error={form.getFieldError(Field(ExplorerTest))}
          onSubmitEditing=onSubmit
        />
        <Buttons.SubmitPrimary
          style=styles##button
          text=I18n.btn#validate_save
          onPress=onSubmit
          disabledLook={!formFieldsAreValids}
        />
      </ColumnRight>
      <ColumnRight />
    </View>
  </Block>;
};
