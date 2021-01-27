open ReactNative;

module RadioItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "container":
          style(
            ~alignSelf=`flexStart,
            ~height=37.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          ),
        "radio": style(~marginRight=16.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (
        ~label,
        ~value,
        ~setValue: (ThemeContext.themeMain => ThemeContext.themeMain) => unit,
        ~currentValue,
      ) => {
    <TouchableOpacity
      style=styles##container onPress={_ => setValue(_ => value)}>
      <Radio value={currentValue == value} style=styles##radio />
      <Typography.Body1> label->React.string </Typography.Body1>
    </TouchableOpacity>;
  };
};

module SettingTextInput = {
  let styles =
    Style.(StyleSheet.create({"input": style(~height=36.->dp, ())}));

  [@react.component]
  let make =
      (~value, ~onValueChange, ~error, ~keyboardType=?, ~onSubmitEditing=?) => {
    <ThemedTextInput
      style=styles##input
      paddingLeft=16.
      paddingVertical=8.
      value
      onValueChange
      hasError={error->Belt.Option.isSome}
      ?keyboardType
      ?onSubmitEditing
    />;
  };
};

module SettingFormGroupTextInput = {
  let styles =
    Style.(
      StyleSheet.create({
        "label": style(~marginBottom=6.->dp, ()),
        "formGroup": style(~marginTop=0.->dp, ~marginBottom=32.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (
        ~label,
        ~value,
        ~onValueChange,
        ~error,
        ~keyboardType=?,
        ~onSubmitEditing=?,
      ) => {
    let hasError = error->Option.isSome;
    <FormGroup style=styles##formGroup>
      <FormLabel label hasError style=styles##label />
      <SettingTextInput
        value
        onValueChange
        error
        ?keyboardType
        ?onSubmitEditing
      />
    </FormGroup>;
  };
};

module ColumnLeft = {
  let styles =
    Style.(
      StyleSheet.create({
        "column": style(~flexGrow=3., ~flexShrink=3., ~flexBasis=0.->dp, ()),
        "columnDouble":
          style(~flexGrow=7., ~flexShrink=7., ~flexBasis=0.->dp, ()),
      })
    );

  [@react.component]
  let make = (~double=false, ~children) => {
    <View style={double ? styles##columnDouble : styles##column}>
      children
    </View>;
  };
};

module ColumnRight = {
  let styles =
    Style.(
      StyleSheet.create({
        "column":
          style(
            ~flexGrow=4.,
            ~flexShrink=4.,
            ~flexBasis=0.->dp,
            ~marginLeft=16.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~children=?) => {
    <View style=styles##column>
      {children->Belt.Option.getWithDefault(React.null)}
    </View>;
  };
};

module Bloc = {
  let styles =
    Style.(
      StyleSheet.create({
        "bloc":
          style(
            ~paddingBottom=32.->dp,
            ~marginBottom=16.->dp,
            ~borderBottomWidth=1.,
            (),
          ),
        "title": style(~marginLeft=16.->dp, ~marginBottom=18.->dp, ()),
        "content": style(~flexDirection=`row, ~paddingHorizontal=30.->dp, ()),
      })
    );

  [@react.component]
  let make = (~title, ~children) => {
    let theme = ThemeContext.useTheme();

    <View
      style=Style.(
        array([|
          styles##bloc,
          style(~borderBottomColor=theme.colors.borderDisabled, ()),
        |])
      )>
      <Typography.Headline fontSize=16. style=styles##title>
        title->React.string
      </Typography.Headline>
      <View style=styles##content> children </View>
    </View>;
  };
};

module BlocVerification = {
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
              custom(
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
                confirmations:
                  state.values.confirmations->Js.String2.length > 0
                    ? Some(state.values.confirmations) : None,
              }
            );
            addToast(
              Logs.info(~origin=Settings, I18n.settings#confirmations_saved),
            );

            None;
          },
        ~initialState={
          confirmations:
            settings.config.confirmations->Option.getWithDefault(""),
        },
        (),
      );

    let onSubmit = _ => {
      form.submit();
    };

    <Bloc title=I18n.settings#confirmations_title>
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
    </Bloc>;
  };
};

module BlocTheme = {
  [@react.component]
  let make = () => {
    let (themeSetting, setThemeSetting) = ThemeContext.useThemeSetting();

    <Bloc title=I18n.settings#theme_title>
      <ColumnLeft>
        <RadioItem
          label=I18n.settings#theme_system
          value=`system
          setValue=setThemeSetting
          currentValue=themeSetting
        />
        <RadioItem
          label=I18n.settings#theme_dark
          value=`dark
          setValue=setThemeSetting
          currentValue=themeSetting
        />
        <RadioItem
          label=I18n.settings#theme_light
          value=`light
          setValue=setThemeSetting
          currentValue=themeSetting
        />
      </ColumnLeft>
    </Bloc>;
  };
};

module BlocChain = {
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
                    ? Some(state.values.endpointTest) : None,
                explorerTest:
                  state.values.explorerTest->Js.String2.length > 0
                    ? Some(state.values.explorerTest) : None,
              }
            );
            addToast(Logs.info(~origin=Settings, I18n.settings#chain_saved));

            None;
          },
        ~initialState={
          endpointTest:
            settings.config.endpointTest->Option.getWithDefault(""),
          explorerTest:
            settings.config.explorerTest->Option.getWithDefault(""),
        },
        (),
      );

    let onSubmit = _ => {
      form.submit();
    };

    <Bloc title=I18n.settings#chain_title>
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
          />
        </ColumnRight>
        <ColumnRight />
      </View>
    </Bloc>;
  };
};

module BlocToken = {
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

    <Bloc title=I18n.settings#token_title>
      <View accessibilityRole=`form style=styles##row>
        <ColumnLeft>
          <Typography.Body1>
            I18n.settings#token_label->React.string
          </Typography.Body1>
        </ColumnLeft>
        <ColumnRight>
          <SettingTextInput
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
    </Bloc>;
  };
};

module BlocDanger = {
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
            //~marginBottom=20.->dp,
            (),
          ),
        "section": style(~marginBottom=8.->dp, ()),
        "button": style(~width=104.->dp, ~height=34.->dp, ()),
      })
    );

  [@react.component]
  let make = () => {
    let onSubmit = _ => {
      Js.log("click");
    };

    let theme = ThemeContext.useTheme();

    <Bloc title=I18n.settings#danger_title>
      <View style=styles##row>
        <ColumnLeft double=true>
          <Typography.Body1 colorStyle=`error style=styles##section>
            I18n.settings#danger_offboard_section->React.string
          </Typography.Body1>
          <Typography.Body1 colorStyle=`error>
            I18n.settings#danger_offboard_text->React.string
          </Typography.Body1>
        </ColumnLeft>
        <ColumnRight>
          <Buttons.SubmitPrimary
            style=Style.(
              array([|
                styles##button,
                style(~backgroundColor=theme.colors.error, ()),
              |])
            )
            text=I18n.settings#danger_offboard_button
            onPress=onSubmit
          />
        </ColumnRight>
      </View>
    </Bloc>;
  };
};

[@react.component]
let make = () => {
  <Page>
    <BlocVerification />
    <BlocTheme />
    <BlocChain />
    <BlocToken />
    <BlocDanger />
  </Page>;
};
