open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M9.000 5.000 L 9.000 7.999 7.020 8.010 L 5.040 8.020 8.520 11.500 L 12.000 14.981 15.480 11.500 L 18.960 8.020 16.980 8.010 L 15.000 7.999 15.000 5.000 L 15.000 2.000 12.000 2.000 L 9.000 2.000 9.000 5.000 M5.000 21.000 L 5.000 22.000 12.000 22.000 L 19.000 22.000 19.000 21.000 L 19.000 20.000 12.000 20.000 L 5.000 20.000 5.000 21.000 " fillRule=`evenodd> </Path> </Svg>;
