type transferOptions = {
  fee: option(ProtocolXTZ.t),
  gasLimit: option(int),
  storageLimit: option(int),
  parameter: option(string),
  entrypoint: option(string),
};

type commonOptions = {
  fee: option(ProtocolXTZ.t),
  burnCap: option(ProtocolXTZ.t),
  forceLowFee: option(bool),
};

let makeTransferOptions =
    (~fee, ~gasLimit, ~storageLimit, ~parameter, ~entrypoint, ()) => {
  fee,
  gasLimit,
  storageLimit,
  parameter,
  entrypoint,
};

let makeCommonOptions = (~fee, ~burnCap, ~forceLowFee, ()) => {
  fee,
  burnCap,
  forceLowFee,
};

let emptyTransferOptions = {
  fee: None,
  gasLimit: None,
  storageLimit: None,
  parameter: None,
  entrypoint: None,
};

type simulationResults = {
  fee: ProtocolXTZ.t,
  gasLimit: int,
  storageLimit: int,
  revealFee: ProtocolXTZ.t,
};
