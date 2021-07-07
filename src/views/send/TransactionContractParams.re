open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~paddingVertical=10.->dp,
          ~paddingHorizontal=16.->dp,
          ~borderRadius=4.,
          (),
        ),
    })
  );

[@react.component]
let make = (~style as styleProp=?, ~parameters) => {
  let theme = ThemeContext.useTheme();
  <View
    style=Style.(
      arrayOption([|
        Some(styles##container),
        Some(style(~backgroundColor=theme.colors.stateRowHovered, ())),
        styleProp,
      |])
    )>
    <Typography.Address>
      {parameters
       ->ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.toString
       ->Option.getWithDefault("")
       ->React.string}
    </Typography.Address>
  </View>;
};
