open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <Path d="M20 12L18.59 10.59 13 16.17 13 4 11 4 11 16.17 5.42 10.58 4 12 12 20z"/> </G> </G> </G> </Svg>
;
