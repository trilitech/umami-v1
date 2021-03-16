open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M11.000 10.090 L 11.000 16.180 8.200 13.380 L 5.400 10.580 4.700 11.280 L 4.000 11.980 8.000 15.981 L 12.000 19.981 16.000 15.981 L 20.000 11.980 19.300 11.280 L 18.600 10.580 15.800 13.380 L 13.000 16.180 13.000 10.090 L 13.000 4.000 12.000 4.000 L 11.000 4.000 11.000 10.090 " fillRule=`evenodd> </Path> </Svg>;
