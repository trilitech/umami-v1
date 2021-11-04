/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;

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

  let url = ReasonReactRouter.useUrl();
  let route = Routes.match(url);

  let (toastState, setToastState) = React.useState(() => None);

  let fadeAnim = React.useRef(Animated.Value.create(1.)).current;

  let (logs, setLogs) = React.useState(() => []);

  let (logs, add, delete, clear) = {
    let delete = (i: int) => {
      setLogs(es => es->List.keepWithIndex((_, i') => i != i'));
    };

    let clear = () => setLogs(_ => []);

    let add = (toast, l) => {
      setLogs(es => {
        let isSimilar =
          switch (List.head(es)->Option.map(fst)) {
          | Some(lastLog) when Logs.similar(lastLog, l) => true
          | _ => false
          };

        isSimilar ? es : [(l, toast), ...es];
      });
    };

    (logs, add, delete, clear);
  };

  React.useEffect1(
    () => {
      logs
      ->List.head
      ->Option.iter(((l, toast)) => {
          if (toast) {
            toastState->Option.map(fst)->Option.iter(Js.Global.clearTimeout);
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

          if (l.Logs.kind == Logs.Error) {
            route == Logs ? (snd(seen))(true) : (snd(seen))(false);
          };
        });

      None;
    },
    [|logs|],
  );

  <Provider value={logs: logs->List.map(fst), add, clear, delete, seen}>
    {toastState->ReactUtils.mapOpt(((_, firsts)) =>
       <ToastBox
         opacity={fadeAnim->Animated.StyleProp.float}
         logs={logs->List.map(fst)}
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
