open ReactNative;
open SettingsComponents;

module StateLenses = [%lenses
  type state = {
    endpointTest: string,
    explorerTest: string,
    endpointMain: string,
    explorerMain: string,
  }
];
module ChainForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "row": style(~flex=1., ~flexDirection=`row, ()),
      "block": style(~flex=1., ~flexDirection=`column, ()),
      "leftcolumntitles": style(~justifyContent=`spaceBetween, ()),
      "chainSeparation": style(~marginTop=30.->dp, ()),
      "button": style(~height=34.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  let writeConf = ConfigContext.useWrite();
  let settings = SdkContext.useSettings();
  let addToast = LogsContext.useToast();

  let checkConfigurationAndContinue = (state: ChainForm.state, k) => {
    Network.checkConfiguration(
      ~network=settings->AppSettings.network,
      state.values.explorerTest,
      state.values.endpointTest,
    )
    ->Future.get(
        fun
        | Ok () => k(state)
        | Error(e) =>
          addToast(Logs.error(~origin=Settings, Network.errorMsg(e))),
      );
  };

  let writeNetwork = f => {
    let network = f(settings->AppSettings.network);
    let network =
      network == ConfigFile.Default.network ? None : Some(network);

    writeConf(c => {...c, network});
  };

  let writeConf = (state: ChainForm.state) =>
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

  let form: ChainForm.api =
    ChainForm.use(
      ~schema={
        ChainForm.Validation.(
          Schema(
            nonEmpty(EndpointTest)
            + nonEmpty(ExplorerTest)
            + nonEmpty(EndpointMain)
            + nonEmpty(ExplorerMain),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          checkConfigurationAndContinue(
            state,
            state => {
              writeConf(state);
              addToast(
                Logs.info(~origin=Settings, I18n.settings#chain_saved),
              );
            },
          );
          None;
        },
      ~initialState={
        endpointTest: settings->AppSettings.endpointTest,
        explorerTest: settings->AppSettings.explorerTest,
        endpointMain: settings->AppSettings.endpointMain,
        explorerMain: settings->AppSettings.explorerMain,
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
      <ColumnLeft style=styles##leftcolumntitles>
        <RadioItem
          label=I18n.t#mainnet
          value=`Mainnet
          setValue=writeNetwork
          currentValue={settings->AppSettings.network}
        />
        <RadioItem
          label=I18n.t#testnet
          value=`Testnet
          setValue=writeNetwork
          currentValue={settings->AppSettings.network}
        />
        <View />
      </ColumnLeft>
      <ColumnRight>
        <SettingFormGroupTextInput
          label=I18n.settings#chain_node_label
          value={form.values.endpointMain}
          onValueChange={form.handleChange(EndpointMain)}
          error={form.getFieldError(Field(EndpointMain))}
          onSubmitEditing=onSubmit
        />
        <SettingFormGroupTextInput
          label=I18n.settings#chain_mezos_label
          value={form.values.explorerMain}
          onValueChange={form.handleChange(ExplorerMain)}
          error={form.getFieldError(Field(ExplorerMain))}
          onSubmitEditing=onSubmit
        />
        <View style=styles##chainSeparation />
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
    </View>
  </Block>;
};
