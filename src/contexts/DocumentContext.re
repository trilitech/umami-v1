open ReactNative;

type callback = Event.pressEvent => unit;
type state = {
  addEventListener: callback => unit,
  removeEventListener: callback => unit,
};

// Context and Provider

let initialState = {addEventListener: _ => (), removeEventListener: _ => ()};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

// Final Provider

let styles = Style.(StyleSheet.create({"document": style(~flex=1., ())}));

[@react.component]
let make = (~children) => {
  let (listeners, setListeners) = React.useState(_ => [||]);

  let addEventListener =
    React.useCallback1(
      cb => {setListeners(listeners => listeners->Belt.Array.concat([|cb|]))},
      //
      [|setListeners|],
    );

  let removeEventListener =
    React.useCallback1(
      cb => {
        setListeners(listeners => listeners->Belt.Array.keep(ls => ls !== cb))
      },
      [|setListeners|],
    );

  let value =
    React.useMemo2(
      () => {{addEventListener, removeEventListener}},
      (addEventListener, removeEventListener),
    );

  let onStartShouldSetResponderCapture =
    React.useCallback0(_pressEvent => true);

  let onResponderRelease =
    React.useCallback1(
      pressEvent => {
        listeners->Belt.Array.forEach(callback => callback(pressEvent))
      },
      [|listeners|],
    );

  <View
    style=styles##document onStartShouldSetResponderCapture onResponderRelease>
    <Provider value> children </Provider>
  </View>;
};

// Hooks

let useDocumentContext = () => React.useContext(context);

let useDocumentPress = callback => {
  let document = useDocumentContext();

  React.useEffect2(
    () => {
      document.addEventListener(callback);
      Some(() => document.removeEventListener(callback));
    },
    (document, callback),
  );
};
