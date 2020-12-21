[@react.component]
let make = (~label, ~hasError=false, ~style: option(ReactNative.Style.t)=?) => {
  <Typography.Overline2 colorStyle=?{hasError ? Some(`error) : None} ?style>
    label->React.string
  </Typography.Overline2>;
};
