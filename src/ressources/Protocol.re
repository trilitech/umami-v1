/** Protocol specific operations */
open ProtocolOptions;

type delegation = {
  source: string,
  delegate: option(string),
  options: commonOptions,
};

type transfer = Transfer.elt;

type transaction = Transfer.t;

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

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  {
    source,
    delegate,
    options: makeCommonOptions(~fee, ~burnCap, ~forceLowFee, ()),
  };
};

let makeTransfer =
    (
      ~destination,
      ~amount,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) =>
  Transfer.makeSingleXTZTransferElt(
    ~destination,
    ~amount,
    ~fee?,
    ~parameter?,
    ~entrypoint?,
    ~gasLimit?,
    ~storageLimit?,
    (),
  );

let makeTransaction =
    (~source, ~transfers, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) =>
  Transfer.makeTransfers(
    ~source,
    ~transfers,
    ~fee?,
    ~burnCap?,
    ~forceLowFee?,
    (),
  )
  ->transfer;

let makeSingleTransaction =
    (
      ~source,
      ~amount,
      ~destination,
      ~burnCap=?,
      ~forceLowFee=?,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) => {
  let transfers = [
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
  ];
  makeTransaction(~source, ~transfers, ~fee?, ~burnCap?, ~forceLowFee?, ());
};

type simulationResults = {
  fee: ProtocolXTZ.t,
  gasLimit: int,
  storageLimit: int,
  revealFee: ProtocolXTZ.t,
};
