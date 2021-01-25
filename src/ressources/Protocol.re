/* 'fee' is duplicated in both option record */
type transfer_options = {
  fee: option(ProtocolXTZ.t),
  gasLimit: option(int),
  storageLimit: option(int),
  parameter: option(string),
  entrypoint: option(string),
};

type common_options = {
  fee: option(ProtocolXTZ.t),
  counter: option(int),
  burnCap: option(ProtocolXTZ.t),
  confirmations: option(int),
  forceLowFee: option(bool),
};

type delegation = {
  source: string,
  delegate: option(string),
  options: common_options,
};

type transfer = {
  amount: ProtocolXTZ.t,
  destination: string,
  tx_options: transfer_options,
};

type transaction = {
  source: string,
  transfers: list(transfer),
  options: common_options,
};

type t =
  | Delegation(delegation)
  | Transaction(transaction);

let transfer = t => t->Transaction;

let emptyTransferOptions = {
  fee: None,
  gasLimit: None,
  storageLimit: None,
  parameter: None,
  entrypoint: None,
};

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

let makeTransfer =
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

let makeTransaction =
    (
      ~source,
      ~transfers,
      ~counter=?,
      ~burnCap=?,
      ~forceLowFee=?,
      ~confirmations=?,
      (),
    ) => {
  source,
  transfers,
  options:
    makeCommonOptions(
      ~fee=None,
      ~counter,
      ~burnCap,
      ~forceLowFee,
      ~confirmations,
      (),
    ),
};

let makeSingleTransaction =
    (
      ~source,
      ~amount,
      ~destination,
      ~counter=?,
      ~burnCap=?,
      ~forceLowFee=?,
      ~confirmations=?,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) => {
  source,
  transfers: [
    makeTransfer(
      ~amount,
      ~destination,
      ~fee?,
      ~parameter?,
      ~entrypoint?,
      ~gasLimit?,
      ~storageLimit?,
      (),
    ),
  ],
  options:
    makeCommonOptions(
      ~fee=None,
      ~counter,
      ~burnCap,
      ~forceLowFee,
      ~confirmations,
      (),
    ),
};

type simulationResults = {
  fee: ProtocolXTZ.t,
  count: int,
  gasLimit: int,
  storageLimit: int,
};
