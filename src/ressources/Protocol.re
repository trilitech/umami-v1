/** Protocol specific operations */
open ProtocolOptions;

type delegation = {
  source: string,
  delegate: option(string),
  options: commonOptions,
};

type t =
  | Delegation(delegation)
  | Transaction(Transfer.t);

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  {
    source,
    delegate,
    options: makeCommonOptions(~fee, ~burnCap, ~forceLowFee, ()),
  };
};

type simulationResults = {
  fee: ProtocolXTZ.t,
  gasLimit: int,
  storageLimit: int,
  revealFee: ProtocolXTZ.t,
};
