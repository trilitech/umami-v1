open ReactNative;

let mapOpt = (v, f) => {
  v->Option.mapWithDefault(React.null, f);
};

let hideNil = (v, f) =>
  switch (v) {
  | [] => React.null
  | v => f(v)
  };

let opt = e =>
  switch (e) {
  | Some(e) => e
  | None => React.null
  };

let displayOn = b => Style.(style(~display=b ? `flex : `none, ()));

let visibleOn = b => Style.(style(~opacity=b ? 1. : 0., ()));

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

let useIsMonted = () => {
  let (isMounted, setIsMounted) = React.useState(_ => false);
  React.useEffect1(
    () => {
      setIsMounted(_ => true);
      None;
    },
    [|setIsMounted|],
  );
  isMounted;
};

let styles = l => Style.array(List.toArray(l));
