let context =
  React.createContext((ProtocolXTZ.zero, (_: ProtocolXTZ.t) => ignore()));

module Provider = {
  let make = React.Context.provider(context);

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
};
