open ReactNative;

module Checkbox = {
  [@react.component]
  let make = (~value: bool=false) => {
    let theme = ThemeContext.useTheme();
    let size = 20.;
    let color = {
      value ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis;
    };
    value
      ? {
        <Icons.CheckboxSelected size color />;
      }
      : {
        <Icons.CheckboxUnselected size color />;
      };
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "pressable":
        style(
          ~alignSelf=`flexStart,
          ~flexDirection=`row,
          ~alignItems=`center,
          (),
        ),
      "checkboxContainer":
        style(
          ~marginRight=11.->dp,
          ~marginLeft=(-5.)->dp,
          ~height=30.->dp,
          ~width=30.->dp,
          ~borderRadius=15.,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~label,
      ~value,
      ~handleChange,
      ~hasError=false,
      ~disabled=false,
      ~labelFontWeightStyle=?,
      ~style as styleFromProp: option(Style.t)=?,
    ) => {
  <Pressable_
    style={_ =>
      Style.arrayOption([|Some(styles##pressable), styleFromProp|])
    }
    onPress={_ => handleChange(!value)}
    disabled
    accessibilityRole=`checkbox>
    {({hovered, pressed, focused}) => {
       let hovered = hovered->Option.getWithDefault(false);
       let focused = focused->Option.getWithDefault(false);
       <>
         <ThemedPressable.ContainerInteractionState.Outline
           hovered pressed focused disabled style=styles##checkboxContainer>
           <Checkbox value />
         </ThemedPressable.ContainerInteractionState.Outline>
         <FormLabel label hasError fontWeightStyle=?labelFontWeightStyle />
       </>;
     }}
  </Pressable_>;
};
