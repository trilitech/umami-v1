open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "spacer":
        style(
          ~height=20.->dp,
          ~alignItems=`flexEnd,
          ~justifyContent=`flexEnd,
          (),
        ),
    })
  );

[@react.component]
let make = (~error=?) => {
  <View style=styles##spacer>
    {error->Option.mapWithDefault(React.null, error =>
       <Typography.Body2 colorStyle=`error fontWeightStyle=`bold>
         error->React.string
       </Typography.Body2>
     )}
  </View>;
};
