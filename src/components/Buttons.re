open ReactNative;
open Common;

let styles =
  Style.(
    StyleSheet.create({
      "primary": style(~borderRadius=4., ()),
      "button":
        style(
          ~display=`flex,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~flexDirection=`row,
          (),
        ),
      "outer": style(~flex=1., ()),
      "pressable":
        style(
          ~flex=1.,
          ~paddingVertical=9.->dp,
          ~paddingHorizontal=17.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderRadius=4.,
          (),
        ),
    })
  );

module FormBase = {
  [@react.component]
  let make =
      (
        ~onPress,
        ~disabled=?,
        ~isPrimary=false,
        ~vStyle=?,
        ~style=?,
        ~children,
      ) => {
    <View style=Style.(arrayOption([|Some(styles##button), vStyle|]))>
      <ThemedPressable
        style={Style.arrayOption([|Some(styles##pressable), style|])}
        outerStyle=styles##outer
        isPrimary
        onPress
        ?disabled>
        children
      </ThemedPressable>
    </View>;
  };
};

module Form = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?style>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle=?{
          disabled->Belt.Option.flatMap(disabled =>
            disabled ? Some(`disabled) : Some(`highEmphasis)
          )
        }>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};

module FormPrimary = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?style>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle=?{
          disabled->Belt.Option.flatMap(disabled =>
            disabled ? Some(`disabled) : Some(`highEmphasis)
          )
        }>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};

module FormSecondary = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?style>
      <Typography.ButtonTernary
        ?fontSize
        colorStyle=?{
          disabled->Belt.Option.flatMap(disabled =>
            disabled ? Some(`disabled) : Some(`highEmphasis)
          )
        }>
        text->React.string
      </Typography.ButtonTernary>
    </FormBase>;
  };
};

module SubmitPrimary = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=false, ~fontSize=?, ~style=?) => {
    let theme = ThemeContext.useTheme();

    <FormBase
      onPress
      isPrimary=true
      disabled
      vStyle={ReactUtils.styles(
        style
        @?
        styles##primary
        @$$ Style.style(
              ~backgroundColor=theme.colors.primaryButtonBackground,
              (),
            ),
      )}>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle={disabled ? `reverseHighEmphasis : `reverseHighEmphasis}>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};

module SubmitSecondary = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=false, ~fontSize=?, ~style=?) => {
    let theme = ThemeContext.useTheme();

    <FormBase
      onPress
      isPrimary=false
      disabled
      vStyle={ReactUtils.styles(
        style
        @?
        styles##primary
        @$$ Style.style(
              ~borderWidth=1.,
              ~borderColor=theme.colors.borderHighEmphasis,
              (),
            ),
      )}>
      <Typography.ButtonPrimary ?fontSize>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};
