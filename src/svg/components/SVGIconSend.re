open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M8.480 5.500 L 4.980 9.000 6.990 9.000 L 9.000 9.000 9.000 12.000 L 9.000 15.000 12.000 15.000 L 15.000 15.000 15.000 12.000 L 15.000 9.000 17.010 9.000 L 19.020 9.000 15.520 5.500 C 13.595 3.575,12.011 2.000,12.000 2.000 C 11.989 2.000,10.405 3.575,8.480 5.500 M5.000 21.000 L 5.000 22.000 12.000 22.000 L 19.000 22.000 19.000 21.000 L 19.000 20.000 12.000 20.000 L 5.000 20.000 5.000 21.000 " fillRule=`evenodd> </Path> </Svg>;
