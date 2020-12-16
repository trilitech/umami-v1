open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~minWidth=160.->dp,
          ~minHeight=46.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
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

[@react.component]
let make = (~text, ~onPress, ~disabled=?, ~fontSize=?, ~style=?) => {
  <View style=styles##button>
    <ThemedPressable
      style={Style.arrayOption([|Some(styles##pressable), style|])}
      onPress
      ?disabled>
      <Typography.ButtonPrimary
        ?fontSize
        colorStyle=?{
          disabled->Belt.Option.flatMap(disabled =>
            disabled ? Some(`disabled) : None
          )
        }>
        text->React.string
      </Typography.ButtonPrimary>
    </ThemedPressable>
  </View>;
};
