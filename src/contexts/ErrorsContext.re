type state = {
  errors: list(Error.t),
  addError: Error.t => unit,
  deleteError: int => unit,
};

let initialState = {errors: [], addError: _ => (), deleteError: _ => ()};

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
  let (errors, addError, deleteError) = {
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

    let delete = (i: int) => {
      setErrors(es => es->Belt.List.keepWithIndex((_, i') => i != i'));
    };

    let add = e => setErrors(es => es->Belt.List.add(e));
    (errors, add, delete);
  };

  <Provider value={errors, addError, deleteError}> children </Provider>;
};

let useStoreContext = () => React.useContext(context);

let useAddError = () => {
  let store = useStoreContext();
  store.addError;
};

let useDeleteError = () => {
  let store = useStoreContext();
  store.deleteError;
};

let useErrors = () => {
  let store = useStoreContext();
  store.errors;
};
