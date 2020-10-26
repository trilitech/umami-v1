open ReactNative;

let styles =
  Style.(StyleSheet.create({"label": style(~marginBottom=6.->dp, ())}));

[@react.component]
let make = (~label, ~hasError=false) => {
  <Typography.Overline1
    colorStyle=?{hasError ? Some(`error) : None} style=styles##label>
    label->React.string
  </Typography.Overline1>;
};
