let context = React.createContext(([||]: array((string, string)), (_: array((string, string))) => ignore()));

module Provider = {
  let make = React.Context.provider(context);

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
};