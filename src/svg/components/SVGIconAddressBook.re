open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg  viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fillRule=`evenodd> <G> <G> <Path d="M17.908 3C19.064 3 20 3.936 20 5.092v14.816C20 21.064 19.064 22 17.908 22H5.987c-.578 0-1.046-.468-1.046-1.046V20.1H4v-.95l.941-.006v-1.898H4v-.945h.941V14.4H4v-.95h.941v-1.9H4v-.946h.941V8.698H4V7.75h.941V5.851H4v-.95h.941v-.855C4.941 3.468 5.41 3 5.987 3h11.921zm-5.432 12.35c-.932 0-1.817.2-2.617.551-.696.304-1.148.988-1.148 1.758v.541h7.53v-.541c0-.77-.453-1.454-1.149-1.758-.8-.351-1.685-.551-2.616-.551zm0-4.75c-1.036 0-1.883.855-1.883 1.9s.847 1.9 1.883 1.9c1.035 0 1.882-.855 1.882-1.9s-.847-1.9-1.882-1.9z"/> </G> </G> </G> </Svg>
;
