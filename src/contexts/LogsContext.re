open ReactNative;
open Common;
type state = {
  logs: list(Logs.t),
  add: Logs.t => unit,
  delete: int => unit,
  clear: unit => unit,
  seen: (bool, bool => unit),
};

let initialState = {
  logs: [],
  add: _ => (),
  delete: _ => (),
  clear: () => (),
  seen: (true, _ => ()),
};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

[@react.component]
let make = (~children) => {
  let seen = {
    let (s, set) = React.useState(() => initialState.seen->fst);
    (s, seen => set(_ => seen));
  };

  let (toastState, setToastState) = React.useState(() => None);

  let fadeAnim = React.useRef(Animated.Value.create(1.)).current;

  let (logs, add, delete, clear) = {
    let (logs, setLogs) = React.useState(() => []);

    let delete = (i: int) => {
      setLogs(es => es->Belt.List.keepWithIndex((_, i') => i != i'));
    };

    let clear = () => setLogs(_ => []);

    let add = l => {
      toastState
      ->Belt.Option.map(fst)
      ->Lib.Option.iter(Js.Global.clearTimeout);
      setLogs(es => es->Belt.List.add(l));
      setToastState(prev => {
        let firsts = prev->Belt.Option.mapWithDefault(0, snd) + 1;
        let animCallback = _ => {
          setToastState(_ => None);
          fadeAnim->Animated.Value.setValue(1.);
        };
        let timeoutCallback = () => {
          ReactUtils.startFade(fadeAnim, 0., 600., Some(animCallback));
        };
        let timeoutid = Js.Global.setTimeout(timeoutCallback, 4500);
        (timeoutid, firsts)->Some;
      });
      l.Logs.kind == Logs.Error ? (snd(seen))(false) : ();
    };

    (logs, add, delete, clear);
  };

  <Provider value={logs, add, clear, delete, seen}>
    {toastState->ReactUtils.mapOpt(((_, firsts)) =>
       <ToastBox
         opacity={fadeAnim->Animated.StyleProp.float}
         logs
         addLog=add
         handleDelete=delete
         firsts
       />
     )}
    children
  </Provider>;
};

let useStoreContext = () => React.useContext(context);

let useAdd = () => {
  let store = useStoreContext();
  store.add;
};

let useSeen = () => {
  let store = useStoreContext();
  store.seen->fst;
};

let useSetSeen = () => {
  let store = useStoreContext();
  store.seen->snd;
};

let useClear = () => {
  let store = useStoreContext();
  store.clear;
};

let useDelete = () => {
  let store = useStoreContext();
  store.delete;
};

let useLogs = () => {
  let store = useStoreContext();
  store.logs;
};
