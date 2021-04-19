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
