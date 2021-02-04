open ReactNative;
open SettingsComponents;

module StateLenses = [%lenses type state = {natviewerTest: string}];
module TokenBalanceForm = ReForm.Make(StateLenses);

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
      "inputAddress":
        style(
          ~fontFamily="JetBrainsMono",
          ~fontSize=14.,
          ~fontWeight=`_200,
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

  let form: TokenBalanceForm.api =
    TokenBalanceForm.use(
      ~schema={
        TokenBalanceForm.Validation.(Schema(nonEmpty(NatviewerTest)));
      },
      ~onSubmit=
        ({state}) => {
          writeConf(c =>
            {
              ...c,
              natviewerTest:
                state.values.natviewerTest->Js.String2.length > 0
                  ? Some(state.values.natviewerTest) : None,
            }
          );
          addToast(Logs.info(~origin=Settings, I18n.settings#token_saved));

          None;
        },
      ~initialState={
        natviewerTest:
          settings.config.natviewerTest->Option.getWithDefault(""),
      },
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <Block title=I18n.settings#token_title>
    <View accessibilityRole=`form style=styles##row>
      <ColumnLeft>
        <Typography.Body1>
          I18n.settings#token_label->React.string
        </Typography.Body1>
      </ColumnLeft>
      <ColumnRight>
        <SettingTextInput
          style=styles##inputAddress
          value={form.values.natviewerTest}
          onValueChange={form.handleChange(NatviewerTest)}
          error={form.getFieldError(Field(NatviewerTest))}
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
