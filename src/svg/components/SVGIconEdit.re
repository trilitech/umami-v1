open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M17.260 3.096 C 17.110 3.168,16.819 3.434,16.100 4.156 L 15.140 5.120 17.010 6.990 L 18.880 8.860 19.859 7.881 C 20.954 6.787,21.022 6.690,20.992 6.283 C 20.983 6.160,20.942 5.988,20.901 5.900 C 20.852 5.795,20.380 5.293,19.523 4.436 C 18.368 3.280,18.198 3.124,18.023 3.066 C 17.745 2.973,17.497 2.983,17.260 3.096 M8.530 11.730 L 3.000 17.260 3.000 19.130 L 3.000 21.000 4.870 21.000 L 6.740 21.000 12.270 15.470 L 17.800 9.940 15.930 8.070 L 14.060 6.200 8.530 11.730 " fillRule=`evenodd> </Path> </Svg>;
