open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <Path fillRule=`nonzero d="M19.158 17.333v.89c0 .977-.758 1.777-1.684 1.777H5.684C4.75 20 4 19.2 4 18.222V5.778C4 4.8 4.75 4 5.684 4h11.79c.926 0 1.684.8 1.684 1.778v.889h-7.58c-.934 0-1.683.8-1.683 1.777v7.112c0 .977.75 1.777 1.684 1.777h7.579zm-7.58-1.777H20V8.444h-8.421v7.112zm3.37-2.223c-.7 0-1.264-.595-1.264-1.333s.564-1.333 1.263-1.333c.7 0 1.264.595 1.264 1.333s-.565 1.333-1.264 1.333z"/> </G> </G> </G> </Svg>
;
