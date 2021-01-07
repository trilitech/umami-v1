open ReactNative;

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
      "loader": style(~position=`absolute, ()),
    })
  );

module FormBase = {
  [@react.component]
  let make =
      (
        ~onPress,
        ~disabled=false,
        ~loading=false,
        ~isPrimary=false,
        ~vStyle=?,
        ~style=?,
        ~children,
      ) => {
    let theme = ThemeContext.useTheme();
    <View style=Style.(arrayOption([|Some(styles##button), vStyle|]))>
      <ThemedPressable
        style={Style.arrayOption([|Some(styles##pressable), style|])}
        outerStyle=styles##outer
        isPrimary
        onPress
        disabled={disabled || loading}>
        {loading
           ? <ActivityIndicator
               animating=true
               size={18.->Style.dp}
               color={
                 isPrimary
                   ? theme.colors.primaryIconMediumEmphasis
                   : theme.colors.iconMediumEmphasis
               }
               style=styles##loader
             />
           : React.null}
        <View style={ReactUtils.visibleOn(!loading)}> children </View>
      </ThemedPressable>
    </View>;
  };
};

module Form = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?loading ?style>
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
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?loading ?style>
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
  let make = (~text, ~onPress, ~disabled=?, ~loading=?, ~fontSize=?, ~style=?) => {
    <FormBase onPress ?disabled ?loading ?style>
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
  let make =
      (
        ~text,
        ~onPress,
        ~disabled=false,
        ~loading=?,
        ~fontSize=?,
        ~style as argStyle=?,
      ) => {
    let theme = ThemeContext.useTheme();

    <FormBase
      onPress
      isPrimary=true
      disabled
      ?loading
      vStyle=Style.(
        arrayOption([|
          argStyle,
          Some(styles##primary),
          Some(
            style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
          ),
        |])
      )>
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
  let make =
      (
        ~text,
        ~onPress,
        ~disabled=false,
        ~loading=?,
        ~fontSize=?,
        ~style as styleArg=?,
      ) => {
    let theme = ThemeContext.useTheme();

    <FormBase
      onPress
      isPrimary=false
      disabled
      ?loading
      vStyle=Style.(
        arrayOption([|
          styleArg,
          Some(styles##primary),
          Some(
            style(
              ~borderWidth=1.,
              ~borderColor=theme.colors.borderHighEmphasis,
              (),
            ),
          ),
        |])
      )>
      <Typography.ButtonPrimary ?fontSize>
        text->React.string
      </Typography.ButtonPrimary>
    </FormBase>;
  };
};
