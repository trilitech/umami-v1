open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <G> <G> <G> <Path d="M5 20L5 22 19 22 19 20z" transform="translate(-1372 -145) translate(147 100) translate(1137 37) translate(80) translate(8 8)"/> <Path d="M19 8L15 8 15 2 9 2 9 8 5 8 12 15z" transform="translate(-1372 -145) translate(147 100) translate(1137 37) translate(80) translate(8 8) matrix(1 0 0 -1 0 17)"/> </G> </G> </G> </G> </G> </G> </Svg>
;
