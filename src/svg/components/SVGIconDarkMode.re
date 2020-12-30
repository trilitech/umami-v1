open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G> <G> <Path fillRule=`nonzero d="M8.77 3c-.97 0-1.893.144-2.77.414C9.748 4.557 12.462 7.968 12.462 12S9.748 19.443 6 20.586c.877.27 1.8.414 2.77.414 5.095 0 9.23-4.032 9.23-9s-4.135-9-9.23-9z" transform="matrix(-1 0 0 1 24 0)"/> </G> </G> </Svg>
;
