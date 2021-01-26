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

module ColumnLeft = {
  let styles =
    Style.(
      StyleSheet.create({
        "column": style(~flexGrow=3., ~flexShrink=3., ~flexBasis=0.->dp, ()),
      })
    );

  [@react.component]
  let make = (~children) => {
    <View style=styles##column> children </View>;
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
        "rowNumberBlock":
          style(
            ~flex=1.,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~marginBottom=20.->dp,
            (),
          ),
        "input": style(~height=36.->dp, ()),
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
            Js.log(state);
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
      <View accessibilityRole=`form style=styles##rowNumberBlock>
        <ColumnLeft>
          <Typography.Body1>
            I18n.settings#confirmations_label->React.string
          </Typography.Body1>
        </ColumnLeft>
        <ColumnRight>
          <ThemedTextInput
            style=styles##input
            value={form.values.confirmations}
            onValueChange={form.handleChange(Confirmations)}
            hasError={
              form.getFieldError(Field(Confirmations))->Belt.Option.isSome
            }
            keyboardType=`numeric
            paddingLeft=16.
            paddingVertical=8.
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

[@react.component]
let make = () => {
  <Page> <BlocVerification /> <BlocTheme /> </Page>;
};
