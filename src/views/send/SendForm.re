module StateLenses = [%lenses
  type state = {
    amount: string,
    sender: option(Account.t),
    recipient: FormUtils.Account.any,
    fee: string,
    gasLimit: string,
    storageLimit: string,
    forceLowFee: bool,
    dryRun: option(Protocol.simulationResults),
  }
];

type validState = {
  amount: Transfer.currency,
  sender: Account.t,
  recipient: FormUtils.Account.t,
  fee: option(ProtocolXTZ.t),
  gasLimit: option(int),
  storageLimit: option(int),
  forceLowFee: bool,
  dryRun: option(Protocol.simulationResults),
};

let unsafeExtractValidState = (token, state: StateLenses.state): validState => {
  {
    amount:
      state.amount
      ->FormUtils.parseAmount(token)
      ->FormUtils.Unsafe.getCurrency,
    sender: state.sender->FormUtils.Unsafe.getValue,
    recipient: state.recipient->FormUtils.Unsafe.account,
    fee: state.fee->ProtocolXTZ.fromString,
    gasLimit: state.gasLimit->Int.fromString,
    storageLimit: state.storageLimit->Int.fromString,
    forceLowFee: state.forceLowFee,
    dryRun: state.dryRun,
  };
};

let toState = (vs: validState): StateLenses.state => {
  amount: vs.amount->Transfer.currencyToString,
  sender: vs.sender->Some,
  recipient: vs.recipient->FormUtils.Account.Valid,

  fee: vs.fee->Option.mapWithDefault("", ProtocolXTZ.toString),
  gasLimit: vs.gasLimit->FormUtils.optToString(Int.toString),
  storageLimit: vs.storageLimit->FormUtils.optToString(Int.toString),
  forceLowFee: vs.forceLowFee,
  dryRun: vs.dryRun,
};

include ReForm.Make(StateLenses);

module Password = {
  module StateLenses = [%lenses type state = {password: string}];

  include ReForm.Make(StateLenses);
};

type transaction = Transfer.t;

let buildTransferElts = (transfers, build) => {
  transfers->List.map(((t: validState, advOpened)) => {
    let destination = t.recipient->FormUtils.Account.address;

    let gasLimit = advOpened ? t.gasLimit : None;
    let storageLimit = advOpened ? t.storageLimit : None;
    let fee = advOpened ? t.fee : None;

    build(
      ~destination,
      ~amount=t.amount,
      ~fee?,
      ~gasLimit?,
      ~storageLimit?,
      (),
    );
  });
};

let buildTransfer = (inputTransfers, source, forceLowFee) =>
  Transfer.makeTransfers(
    ~source=source.Account.address,
    ~transfers=
      buildTransferElts(
        inputTransfers,
        Transfer.makeSingleTransferElt(~parameter=?None, ~entrypoint=?None),
      ),
    ~forceLowFee?,
    (),
  );

let buildTransaction = (batch: list((validState, bool))) => {
  switch (batch) {
  | [] => assert(false)
  | [(first, _), ..._] as inputTransfers =>
    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;

    buildTransfer(inputTransfers, source, forceLowFee);
  };
};
