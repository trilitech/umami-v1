open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <Path d="M8 0c4.418 0 8 3.582 8 8s-3.582 8-8 8-8-3.582-8-8 3.582-8 8-8zm2.3 5.5H5.7c-.11 0-.2.09-.2.2v4.6c0 .11.09.2.2.2h4.6c.11 0 .2-.09.2-.2V5.7c0-.11-.09-.2-.2-.2z" transform="translate(4 4)"/> </G> </G> </G> </Svg>
;
