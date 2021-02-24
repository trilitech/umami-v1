open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M5.710 5.710 L 5.000 6.420 7.790 9.210 L 10.580 12.000 7.790 14.790 L 5.000 17.580 5.710 18.290 L 6.420 19.000 9.210 16.210 L 12.000 13.420 14.790 16.210 L 17.580 19.000 18.290 18.290 L 19.000 17.580 16.210 14.790 L 13.420 12.000 16.210 9.210 L 19.000 6.420 18.290 5.710 L 17.580 5.000 14.790 7.790 L 12.000 10.580 9.210 7.790 L 6.420 5.000 5.710 5.710 " fillRule=`evenodd> </Path> </Svg>;
