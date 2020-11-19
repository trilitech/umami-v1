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
  forceLowFee: option(bool),
};

type delegation = {
  source: string,
  delegate: string,
  fee: option(float),
  burnCap: option(float),
  forceLowFee: option(bool),
};

type operation =
  | Transaction(transaction)
  | Delegation(delegation);

let makeTransfer =
    (
      ~source,
      ~amount,
      ~destination,
      ~fee=?,
      ~counter=?,
      ~gasLimit=?,
      ~storageLimit=?,
      ~burnCap=?,
      ~confirmations=?,
      ~forceLowFee=?,
      (),
    ) => {
  Transaction({
    source,
    amount,
    destination,
    fee,
    counter,
    gasLimit,
    storageLimit,
    burnCap,
    confirmations,
    forceLowFee,
  });
};

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  Delegation({source, delegate, fee, burnCap, forceLowFee});
};
