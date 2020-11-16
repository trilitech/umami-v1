open ReactNative;

[@react.component]
let make = (~style: option(Style.t)=?, ~value: bool=false) => {
  let size = 24.;
  let color = {
    value ? "#FFF" : Theme.colorDarkMediumEmphasis;
  };
  value
    ? {
      <Icons.CheckboxSelected size color ?style />;
    }
    : {
      <Icons.CheckboxUnselected size color ?style />;
    };
};
