open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 14 14" ?width ?height ?fill ?stroke> <Path d="M8.334 6.354 L 5.250 9.438 4.025 8.213 L 2.800 6.988 2.386 7.405 L 1.972 7.821 3.605 9.440 C 4.503 10.331,5.244 11.060,5.250 11.060 C 5.256 11.060,6.834 9.487,8.757 7.565 L 12.251 4.070 11.835 3.670 L 11.418 3.270 8.334 6.354 " fillRule=`evenodd> </Path> </Svg>;
