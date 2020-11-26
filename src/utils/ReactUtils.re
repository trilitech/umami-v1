open ReactNative;

let mapOpt = (v, f) => {
  v->Belt.Option.mapWithDefault(React.null, f);
};

let displayOn = b => Style.(style(~display=b ? `flex : `none, ()));

let onlyWhen = (elt, b) => b ? elt : React.null;

let startFade = (refval, endval, duration, endCallback) => {
  Animated.(
    Value.Timing.(
      timing(
        refval,
        config(
          ~toValue=fromRawValue(endval),
          ~duration,
          ~useNativeDriver=true,
          (),
        ),
      )
      ->start(~endCallback?, ())
    )
  );
};
