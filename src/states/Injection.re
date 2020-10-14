type transaction = {
  source: string,
  amount: float,
  destination: string,
  fee: option(float),
  counter: option(int),
  gasLimit: option(int),
  storageLimit: option(int),
  burnCap: option(float),
  confirmations: option(int),
  forceLowFee: option(bool)
};

let makeTransfer = (~source, ~amount, ~destination, ~fee=?, ~counter=?, ~gasLimit=?, ~storageLimit=?, ~burnCap=?, ~confirmations=?, ~forceLowFee=?, ()) => {
  {
    source,
    amount,
    destination,
    fee,
    counter,
    gasLimit,
    storageLimit,
    burnCap,
    confirmations,
    forceLowFee
  }
}

type delegation = {
  source: string,
  delegate: string,
};

type operation =
  | Transaction(transaction)
  | Delegation(delegation);

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
