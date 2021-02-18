open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M11.000 8.000 L 11.000 11.000 8.000 11.000 L 5.000 11.000 5.000 12.000 L 5.000 13.000 8.000 13.000 L 11.000 13.000 11.000 16.000 L 11.000 19.000 12.000 19.000 L 13.000 19.000 13.000 16.000 L 13.000 13.000 16.000 13.000 L 19.000 13.000 19.000 12.000 L 19.000 11.000 16.000 11.000 L 13.000 11.000 13.000 8.000 L 13.000 5.000 12.000 5.000 L 11.000 5.000 11.000 8.000 " fillRule=`evenodd> </Path> </Svg>;
