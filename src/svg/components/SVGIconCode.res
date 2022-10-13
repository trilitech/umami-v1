open ReactNative
open ReactNativeSvg

@react.component
let make = (
  ~width: option<Style.size>=?,
  ~height: option<Style.size>=?,
  ~fill: option<string>=?,
  ~stroke: option<string>=?,
) =>
  <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke>
    <Path
      d="m11.99 18.612-7.37-5.409L3 14.393 12 21l9-6.607-1.63-1.199-7.38 5.418zm.01-2.398 7.36-5.408L21 9.607 12 3 3 9.607l1.63 1.199L12 16.214z"
      fillRule=#evenodd
    />
  </Svg>
