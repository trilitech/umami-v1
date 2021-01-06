open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <Path fillRule=`nonzero d="M18.222 18.222H5.778V5.778H12V4H5.778C4.79 4 4 4.8 4 5.778v12.444C4 19.2 4.791 20 5.778 20h12.444C19.2 20 20 19.2 20 18.222V12h-1.778v6.222zM13.778 4v1.778h3.19l-8.737 8.738 1.253 1.253 8.738-8.738v3.191H20V4h-6.222z"/> </G> </G> </G> </Svg>
;
