open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <G> <Path fillRule=`nonzero d="M16.59 8.59L12 13.17 7.41 8.59 6 10 12 16 18 10z" transform="matrix(0 1 1 0 0 0) matrix(0 1 1 0 -.295 .295)"/> </G> </G> </G> </G> </Svg>
;
