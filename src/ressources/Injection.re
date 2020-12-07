type transaction_options = {
  fee: option(float),
  counter: option(int),
  gasLimit: option(int),
  storageLimit: option(int),
  burnCap: option(float),
  confirmations: option(int),
  forceLowFee: option(bool),
};

type transaction = {
  source: string,
  amount: float,
  destination: string,
  options: transaction_options,
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

let makeTransferOptions =
    (
      ~fee,
      ~counter,
      ~gasLimit,
      ~storageLimit,
      ~burnCap,
      ~confirmations,
      ~forceLowFee,
      (),
    ) => {
  fee,
  counter,
  gasLimit,
  storageLimit,
  burnCap,
  confirmations,
  forceLowFee,
};

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
    options:
      makeTransferOptions(
        ~fee,
        ~counter,
        ~gasLimit,
        ~storageLimit,
        ~burnCap,
        ~confirmations,
        ~forceLowFee,
        (),
      ),
  });
};

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  Delegation({source, delegate, fee, burnCap, forceLowFee});
};
