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
  amount: FormUtils.strictAmount,
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
  amount: vs.amount->FormUtils.amountToString,
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

type transaction =
  | ProtocolTransaction(Protocol.transaction)
  | TokenTransfer(Token.Transfer.t, Token.t);

let toOperation = (t: transaction) =>
  switch (t) {
  | ProtocolTransaction(transaction) => Operation.transaction(transaction)
  | TokenTransfer(transfer, _) => Operation.Token(transfer->Token.transfer)
  };

let toSimulation = (~index=?, t: transaction) =>
  switch (t) {
  | ProtocolTransaction(transaction) =>
    Operation.Simulation.transaction(transaction, index)
  | TokenTransfer(transfer, _) =>
    Operation.Simulation.Token(transfer->Token.transfer, index)
  };

let buildTransfers = (transfers, parseAmount, build) => {
  transfers->List.keepMap(((t: validState, advOpened)) => {
    let destination = t.recipient->FormUtils.Account.address;

    let gasLimit = advOpened ? t.gasLimit : None;
    let storageLimit = advOpened ? t.storageLimit : None;
    let fee = advOpened ? t.fee : None;

    let amount = parseAmount(t.amount);

    amount->Option.map(amount =>
      build(~destination, ~amount, ~fee?, ~gasLimit?, ~storageLimit?, ())
    );
  });
};

let buildTokenTransfer = (inputTransfers, token: Token.t, source, forceLowFee) =>
  TokenTransfer(
    Token.makeTransfers(
      ~source=source.Account.address,
      ~transfers=
        buildTransfers(
          inputTransfers,
          v => v->FormUtils.keepStrictToken->Option.map(fst),
          Token.makeSingleTransferElt(~token=token.address),
        ),
      ~forceLowFee?,
      (),
    ),
    token,
  );

let buildProtocolTransaction = (inputTransfers, source, forceLowFee) =>
  Protocol.makeTransaction(
    ~source=source.Account.address,
    ~transfers=
      buildTransfers(
        inputTransfers,
        FormUtils.keepStrictXTZ,
        Protocol.makeTransfer(~parameter=?None, ~entrypoint=?None),
      ),
    ~forceLowFee?,
    (),
  )
  ->ProtocolTransaction;

let buildTransaction =
    (batch: list((validState, bool)), token: option(Token.t)) => {
  switch (batch) {
  | [] => assert(false)
  | [(first, _), ..._] as inputTransfers =>
    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;

    switch (token) {
    | Some(token) =>
      buildTokenTransfer(inputTransfers, token, source, forceLowFee)
    | None => buildProtocolTransaction(inputTransfers, source, forceLowFee)
    };
  };
};
