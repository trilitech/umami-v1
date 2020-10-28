open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <Path d="M19 13L13 13 13 19 11 19 11 13 5 13 5 11 11 11 11 5 13 5 13 11 19 11z"/> </G> </G> </Svg>
;
