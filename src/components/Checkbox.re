open ReactNative;

[@react.component]
let make =
    (
      ~style: option(Style.t)=?,
      ~value: bool=false,
      ~onValueChange: option(bool => unit)=?,
      ~disabled: bool=false,
    ) => {
  <Icon
    name={value ? `checkboxSelected : `checkboxUnselected}
    size=24.
    color={value ? "#FFF" : Theme.colorDarkMediumEmphasis}
    ?style
  />;
};
