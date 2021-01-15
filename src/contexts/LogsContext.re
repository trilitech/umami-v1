open ReactNative;
open UmamiCommon;
type state = {
  logs: list(Logs.t),
  add: (bool, Logs.t) => unit,
  delete: int => unit,
  clear: unit => unit,
  seen: (bool, bool => unit),
};

let initialState = {
  logs: [],
  add: (_, _) => (),
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
      setLogs(es => es->List.keepWithIndex((_, i') => i != i'));
    };

    let clear = () => setLogs(_ => []);

    let add = (toast, l) => {
      setLogs(es => es->List.add(l));

      if (toast) {
        toastState->Option.map(fst)->Lib.Option.iter(Js.Global.clearTimeout);
        setToastState(prev => {
          let firsts = prev->Option.mapWithDefault(0, snd) + 1;
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
      };

      l.Logs.kind == Logs.Error ? (snd(seen))(false) : ();
    };

    (logs, add, delete, clear);
  };

  <Provider value={logs, add, clear, delete, seen}>
    {toastState->ReactUtils.mapOpt(((_, firsts)) =>
       <ToastBox
         opacity={fadeAnim->Animated.StyleProp.float}
         logs
         addToast={add(false)}
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

let useToast = () => {
  let store = useStoreContext();
  store.add(true);
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
