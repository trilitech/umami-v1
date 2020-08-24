type transaction = {
  source: string,
  amount: float,
  destination: string,
};

type status =
  | Pending(transaction)
  | Done;

let context = React.createContext((Done, (_: status) => ignore()));

module Provider = {
  let make = React.Context.provider(context);

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
};