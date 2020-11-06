let mapOpt = (v, f) => {
  v->Belt.Option.mapWithDefault(React.null, f);
};
