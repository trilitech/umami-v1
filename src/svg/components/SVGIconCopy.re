open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <Path fillRule=`nonzero d="M15.455 3H5.636C4.736 3 4 3.736 4 4.636v11.455h1.636V4.636h9.819V3zm2.454 3.273h-9c-.9 0-1.636.736-1.636 1.636v11.455c0 .9.736 1.636 1.636 1.636h9c.9 0 1.636-.736 1.636-1.636V7.909c0-.9-.736-1.636-1.636-1.636zm0 13.09h-9V7.91h9v11.455z"/> </G> </G> </G> </Svg>
;
