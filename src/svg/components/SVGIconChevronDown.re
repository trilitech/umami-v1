open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M6.990 8.990 C 6.621 9.359,6.320 9.679,6.320 9.701 C 6.320 9.751,12.270 15.703,12.297 15.678 C 12.308 15.668,13.658 14.319,15.298 12.680 L 18.280 9.700 17.580 9.000 L 16.880 8.300 14.590 10.590 L 12.300 12.880 10.020 10.600 C 8.766 9.346,7.722 8.320,7.700 8.320 C 7.677 8.320,7.358 8.622,6.990 8.990 " fillRule=`evenodd> </Path> </Svg>;
