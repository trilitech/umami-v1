let context =
  React.createContext((
    AppSettings.Testnet,
    (_: AppSettings.network) => ignore(),
  ));

module Provider = {
  let make = React.Context.provider(context);

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
};
