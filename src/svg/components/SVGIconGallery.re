open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 14 14" ?width ?height ?fill ?stroke> <Path d="M4.072 3.223 L 1.178 5.251 1.172 5.834 L 1.166 6.417 1.750 6.417 L 2.333 6.417 2.333 9.042 L 2.333 11.667 1.750 11.667 L 1.167 11.667 1.167 12.250 L 1.167 12.833 7.000 12.833 L 12.833 12.833 12.833 12.250 L 12.833 11.667 12.250 11.667 L 11.667 11.667 11.667 9.042 L 11.667 6.417 12.250 6.417 L 12.833 6.417 12.833 5.838 L 12.833 5.259 9.929 3.225 C 8.332 2.106,7.011 1.191,6.995 1.192 C 6.978 1.193,5.663 2.107,4.072 3.223 M10.500 8.458 L 10.500 11.667 7.000 11.667 L 3.500 11.667 3.500 8.458 L 3.500 5.250 7.000 5.250 L 10.500 5.250 10.500 8.458 M5.526 6.784 C 5.425 6.894,4.722 7.780,4.692 7.836 C 4.677 7.865,4.670 7.906,4.677 7.928 C 4.684 7.950,5.191 8.470,5.804 9.083 C 6.845 10.122,6.925 10.197,7.000 10.197 C 7.075 10.197,7.155 10.122,8.185 9.094 C 8.793 8.488,9.300 7.967,9.312 7.938 C 9.342 7.861,9.319 7.826,8.850 7.245 L 8.435 6.732 7.010 6.726 L 5.586 6.720 5.526 6.784 " fillRule=`evenodd> </Path> </Svg>;
