open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "pressable":
        style(
          ~flexDirection=`row,
          ~justifyContent=`spaceBetween,
          ~alignItems=`center,
          ~paddingVertical=8.->dp,
          ~marginVertical=10.->dp,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~label,
      ~value,
      ~setValue: ('a => 'a) => unit,
      ~disabled=false,
      ~style as styleFromProp: option(Style.t)=?,
    ) => {
  <Pressable_
    style={_ =>
      Style.arrayOption([|Some(styles##pressable), styleFromProp|])
    }
    onPress={_ => setValue(_ => !value)}
    disabled
    accessibilityRole=`checkbox>
    {({hovered, pressed, focused}) => {
       let hovered = hovered->Option.getWithDefault(false);
       let focused = focused->Option.getWithDefault(false);
       <>
         <Typography.Overline2> label->React.string </Typography.Overline2>
         <ThemedSwitch value hovered pressed focused disabled />
       </>;
     }}
  </Pressable_>;
};
