open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 36 36" ?width ?height ?fill ?stroke> <G> <G> <G transform="translate(-419 -91) translate(399 71) translate(8 8)"> <G> <G> <Path fillRule=`nonzero d="M28.5 16.5H11.745l7.32-7.32c.585-.585.585-1.545 0-2.13s-1.53-.585-2.115 0l-9.885 9.885c-.585.585-.585 1.53 0 2.115l9.885 9.885c.585.585 1.53.585 2.115 0 .585-.585.585-1.53 0-2.115l-7.32-7.32H28.5c.825 0 1.5-.675 1.5-1.5s-.675-1.5-1.5-1.5z" transform="translate(12 12)"/> </G> </G> </G> </G> </G> </Svg>
;
