type state = {
  errors: list(Error.t),
  addError: Error.t => unit,
};

let initialState = {errors: [], addError: _ => ()};

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
  let (errors, addError) = {
    let (errors, setErrors) =
      React.useState(() =>
        [
          Error.{
            kind: Operation,
            msg: "this transaction was not permitted",
            timestamp: Js.Date.now(),
          },
          Error.{
            kind: Connection,
            msg: "broken connection of your wallet babe",
            timestamp: Js.Date.now() +. 1.,
          },
        ]
      );
    let add = e => setErrors(es => es->Belt.List.add(e));
    (errors, add);
  };

  <Provider value={errors, addError}> children </Provider>;
};

let useStoreContext = () => React.useContext(context);

let useAddError = () => {
  let store = useStoreContext();
  store.addError;
};

let useErrors = () => {
  let store = useStoreContext();
  store.errors;
};
