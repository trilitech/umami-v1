open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "label": style(~marginBottom=6.->dp, ()),
      "labelError": style(~color=Colors.error, ()),
    })
  );

[@react.component]
let make = (~label, ~hasError=false) => {
  <Typography.Overline1
    style=Style.(
      arrayOption([|
        Some(styles##label),
        hasError ? Some(styles##labelError) : None,
      |])
    )>
    label->React.string
  </Typography.Overline1>;
};
