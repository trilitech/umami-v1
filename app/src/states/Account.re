let context = React.createContext(("tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3", (_: string) => ignore()));

module Provider = {
  let make = React.Context.provider(context);

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
};