/* 'fee' is duplicated in both option record */
type transfer_options = {
  fee: option(float),
  gasLimit: option(int),
  storageLimit: option(int),
  parameter: option(string),
  entrypoint: option(string),
};

type common_options = {
  fee: option(float),
  counter: option(int),
  burnCap: option(float),
  confirmations: option(int),
  forceLowFee: option(bool),
};

type transfer = {
  source: string,
  amount: float,
  destination: string,
  tx_options: transfer_options,
  common_options,
};

type delegation = {
  source: string,
  delegate: string,
  options: common_options,
};

type single_batch_transaction = {
  amount: float,
  destination: string,
  tx_options: transfer_options,
};

type batch_transactions = {
  source: string,
  transactions: array(single_batch_transaction),
  options: common_options,
};

type t =
  | Transfer(transfer)
  | Delegation(delegation)
  | BatchTransactions(batch_transactions);

let transfer = t => t->Transfer;

let makeTransferOptions =
    (~fee, ~gasLimit, ~storageLimit, ~parameter, ~entrypoint, ()) => {
  fee,
  gasLimit,
  storageLimit,
  parameter,
  entrypoint,
};

let makeCommonOptions =
    (~fee, ~counter, ~burnCap, ~confirmations, ~forceLowFee, ()) => {
  fee,
  counter,
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
  {
    source,
    amount,
    destination,
    tx_options:
      makeTransferOptions(
        ~fee,
        ~gasLimit,
        ~storageLimit,
        ~parameter=None,
        ~entrypoint=None,
        (),
      ),
    common_options:
      makeCommonOptions(
        ~fee,
        ~counter,
        ~burnCap,
        ~confirmations,
        ~forceLowFee,
        (),
      ),
  };
};

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ~counter=?, ()) => {
  {
    source,
    delegate,
    options:
      makeCommonOptions(
        ~fee,
        ~burnCap,
        ~forceLowFee,
        ~counter,
        ~confirmations=None,
        (),
      ),
  };
};

let makeSingleBatchTransfer =
    (
      ~amount,
      ~destination,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) => {
  amount,
  destination,
  tx_options:
    makeTransferOptions(
      ~fee,
      ~parameter,
      ~entrypoint,
      ~gasLimit,
      ~storageLimit,
      (),
    ),
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
    options:
      makeCommonOptions(
        ~fee=None,
        ~counter,
        ~burnCap,
        ~forceLowFee,
        ~confirmations,
        (),
      ),
  });

let simulableSingleBatchTransfer = (~source, ~singleBatchTransaction, ()) =>
  makeTransfer(
    ~source,
    ~destination=singleBatchTransaction.destination,
    ~amount=singleBatchTransaction.amount,
    (),
  );

type simulationResults = {
  fee: float,
  count: int,
  gasLimit: int,
  storageLimit: int,
};
