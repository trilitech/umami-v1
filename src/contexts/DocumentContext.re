open ReactNative;

type element = React.ref(Js.Nullable.t(NativeElement.element));
type callback = Event.pressEvent => unit;
type state = {
  addEventListener: (element, bool, callback) => unit,
  removeEventListener: (element, bool, callback) => unit,
};

// Context and Provider

let initialState = {
  addEventListener: (_, _, _) => (),
  removeEventListener: (_, _, _) => (),
};

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
    React.useMemo1(
      ((), element, isOpen, callback) => {
        setListeners(listeners =>
          listeners->Belt.Array.concat([|(element, isOpen, callback)|])
        )
      },
      [|setListeners|],
    );

  let removeEventListener =
    React.useMemo1(
      ((), _element, _isOpen, callback) => {
        setListeners(listeners =>
          listeners->Belt.Array.keep(((_, _, cb)) => cb !== callback)
        )
      },
      [|setListeners|],
    );

  let value =
    React.useMemo2(
      () => {{addEventListener, removeEventListener}},
      (addEventListener, removeEventListener),
    );

  let onStartShouldSetResponderCapture =
    React.useCallback1(
      pressEvent => {
        listeners->Belt.Array.some(((element, isOpen, _)) => {
          isOpen
          && element.current !==
          pressEvent->Event.PressEvent.nativeEvent##target
        })
      },
      [|listeners|],
    );

  let onResponderRelease =
    React.useCallback1(
      pressEvent => {
        listeners->Belt.Array.forEach(((_element, _isOpen, callback)) =>
          callback(pressEvent)
        )
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

let useClickOutside = (element, isOpen, callback) => {
  let document = useDocumentContext();

  React.useEffect3(
    () => {
      document.addEventListener(element, isOpen, callback);
      Some(() => document.removeEventListener(element, isOpen, callback));
    },
    (document, isOpen, callback),
  );
};
