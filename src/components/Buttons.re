open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "primary": style(~borderRadius=4., ()),
      "button": style(~alignItems=`center, ~justifyContent=`center, ()),
      "pressable":
        style(
          ~paddingVertical=9.->dp,
          ~paddingHorizontal=17.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderRadius=5.,
          (),
        ),
    })
  );

module Base = {
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
        isPrimary
        onPress
        ?disabled>
        children
      </ThemedPressable>
    </View>;
  };
};

module FormPrimary = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=?, ~fontSize=?, ~style=?) => {
    <Base onPress ?disabled ?style>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle=?{
          disabled->Belt.Option.flatMap(disabled =>
            disabled ? Some(`disabled) : Some(`highEmphasis)
          )
        }>
        text->React.string
      </Typography.ButtonPrimary>
    </Base>;
  };
};

module SubmitPrimary = {
  [@react.component]
  let make = (~text, ~onPress, ~disabled=false, ~fontSize=?, ~style=?) => {
    let theme = ThemeContext.useTheme();

    <Base
      onPress
      isPrimary=true
      disabled
      ?style
      vStyle=Style.(
        array([|
          styles##primary,
          style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
        |])
      )>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle={disabled ? `reverseHighEmphasis : `reverseHighEmphasis}>
        text->React.string
      </Typography.ButtonPrimary>
    </Base>;
  };
};
