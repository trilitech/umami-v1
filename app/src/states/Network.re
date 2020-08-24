type name =
  | Main
  | Test;

let context = React.createContext((Test, (_: name) => ignore()));

module Provider = {
  let make = React.Context.provider(context);

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
};