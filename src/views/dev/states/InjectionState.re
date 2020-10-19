include Injection;

type status =
  | Pending(operation)
  | Done;

let context = React.createContext((Done, (_: status) => ignore()));

module Provider = {
  let make = React.Context.provider(context);

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
};
