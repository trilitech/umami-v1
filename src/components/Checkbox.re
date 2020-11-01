open ReactNative;

[@react.component]
let make = (~style: option(Style.t)=?, ~value: bool=false) => {
  <Icon
    name={value ? `checkboxSelected : `checkboxUnselected}
    size=24.
    color={value ? "#FFF" : Theme.colorDarkMediumEmphasis}
    ?style
  />;
};
