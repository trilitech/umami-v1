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

type batch_transactions = {
  source: string,
  transactions: array(transaction),
  counter: option(int),
  burnCap: option(float),
  forceLowFee: option(bool),
  confirmations: option(int),
};

type operation =
  | Transaction(transaction)
  | Delegation(delegation)
  | BatchTransactions(batch_transactions);

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

let makeBatchTransfers =
    (
      ~source,
      ~transactions,
      ~counter=?,
      ~burnCap=?,
      ~forceLowFee=?,
      ~confirmations=?,
      (),
    ) =>
  BatchTransactions({
    source,
    transactions,
    counter,
    burnCap,
    forceLowFee,
    confirmations,
  });
