module MapString = Belt.Map.String;

let context = React.createContext((MapString.empty: MapString.t(string), (_: MapString.t(string)) => ignore()));

module Provider = {
  let make = React.Context.provider(context);

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
};