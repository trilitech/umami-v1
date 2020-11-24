open ReactNative;
open Common;
type state = {
  errors: list(Error.t),
  add: Error.t => unit,
  delete: int => unit,
  clear: unit => unit,
  seen: (bool, bool => unit),
};

let initialState = {
  errors: [],
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

  let (errors, add, delete, clear) = {
    let (errors, setErrors) = React.useState(() => []);

    let delete = (i: int) => {
      setErrors(es => es->Belt.List.keepWithIndex((_, i') => i != i'));
    };

    let clear = () => setErrors(_ => []);

    let add = e => {
      toastState
      ->Belt.Option.map(fst)
      ->Lib.Option.iter(Js.Global.clearTimeout);
      setErrors(es => es->Belt.List.add(e));
      setToastState(prev => {
        let firsts = prev->Belt.Option.mapWithDefault(0, snd) + 1;
        let animCallback = _ => setToastState(_ => None);
        let timeoutCallback = () => {
          ReactUtils.startFade(fadeAnim, 0., 600., Some(animCallback));
        };
        let timeoutid = Js.Global.setTimeout(timeoutCallback, 5000);
        (timeoutid, firsts)->Some;
      });
      (snd(seen))(false);
    };

    (errors, add, delete, clear);
  };

  <Provider value={errors, add, clear, delete, seen}>
    {toastState->ReactUtils.mapOpt(((_, firsts)) =>
       <ToastBox
         opacity={fadeAnim->Animated.StyleProp.float}
         errors
         handleDelete=delete
         firsts
       />
     )}
    children
  </Provider>;
};

let useStoreContext = () => React.useContext(context);

let useAddError = () => {
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

let useDeleteError = () => {
  let store = useStoreContext();
  store.delete;
};

let useErrors = () => {
  let store = useStoreContext();
  store.errors;
};
