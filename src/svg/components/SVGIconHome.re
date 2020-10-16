open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <G> <Path d="M10 20L10 14 14 14 14 20 19 20 19 12 22 12 12 3 2 12 5 12 5 20z" transform="translate(-43 -80) translate(22 80) translate(21)"/> </G> </G> </G> </G> </Svg>
;
