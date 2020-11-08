let mapOpt = (v, f) => {
  v->Belt.Option.mapWithDefault(React.null, f);
};

let displayOn = b =>
  ReactNative.Style.(style(~display=b ? `flex : `none, ()));
