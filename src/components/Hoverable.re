open ReactNative;

[@react.component]
let make = (~style as stylearg=?, ~hoveredStyle=?, ~children) => {
  let (hovered, setHovered) = React.useState(_ => false);
  <View
    onMouseEnter={_ => setHovered(_ => true)}
    onMouseLeave={_ => setHovered(_ => false)}
    onResponderGrant={_ => setHovered(_ => false)}
    onResponderRelease={_ => setHovered(_ => true)}
    style=Style.(arrayOption([|stylearg, hovered ? hoveredStyle : None|]))>
    {children(hovered)}
  </View>;
};
