module StateLenses = [%lenses
  type state = {
    amount: string,
    sender: string,
    recipient: FormUtils.Account.t,
    fee: string,
    gasLimit: string,
    storageLimit: string,
    forceLowFee: bool,
    dryRun: option(Protocol.simulationResults),
  }
];

type validState = {
  amount: FormUtils.strictAmount,
  sender: string,
  recipient: FormUtils.Account.t,
  fee: option(ProtocolXTZ.t),
  gasLimit: string,
  storageLimit: string,
  forceLowFee: bool,
  dryRun: option(Protocol.simulationResults),
};

let unsafeExtractValidState = (token, state: StateLenses.state): validState => {
  {
    amount:
      state.amount
      ->FormUtils.parseAmount(token)
      ->FormUtils.Unsafe.getCurrency,
    sender: state.sender,
    recipient: state.recipient,
    fee: state.fee->ProtocolXTZ.fromString,
    gasLimit: state.gasLimit,
    storageLimit: state.storageLimit,
    forceLowFee: state.forceLowFee,
    dryRun: state.dryRun,
  };
};

let toState = (vs: validState): StateLenses.state => {
  amount: vs.amount->FormUtils.amountToString,
  sender: vs.sender,
  recipient: vs.recipient,
  fee: vs.fee->Option.mapWithDefault("", ProtocolXTZ.toString),
  gasLimit: vs.gasLimit,
  storageLimit: vs.storageLimit,
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

let resolveAlias = (accounts, value) =>
  accounts
  ->Map.String.findFirstBy((_, v: Account.t) => v.alias == value)
  ->Option.mapWithDefault(value, ((k, _)) => k);

let buildTransfers = (transfers, parseAmount, accounts, build) => {
  transfers->List.keepMap(((t: validState, advOpened)) => {
    let mapIfAdvanced = (v, flatMap) =>
      advOpened && v->Js.String2.length > 0 ? v->flatMap : None;

    let destination =
      resolveAlias(accounts, t.recipient->FormUtils.Account.address);

    let gasLimit = t.gasLimit->mapIfAdvanced(Int.fromString);
    let storageLimit = t.storageLimit->mapIfAdvanced(Int.fromString);
    let fee = advOpened ? t.fee : None;

    let amount = parseAmount(t.amount);

    amount->Option.map(amount =>
      build(~destination, ~amount, ~fee?, ~gasLimit?, ~storageLimit?, ())
    );
  });
};

let buildTokenTransfer =
    (inputTransfers, token: Token.t, accounts, source, forceLowFee) =>
  TokenTransfer(
    Token.makeTransfers(
      ~source,
      ~transfers=
        buildTransfers(
          inputTransfers,
          v => v->FormUtils.keepStrictToken->Option.map(fst),
          accounts,
          Token.makeSingleTransferElt(~token=token.address),
        ),
      ~forceLowFee?,
      (),
    ),
    token,
  );

let buildProtocolTransaction = (inputTransfers, accounts, source, forceLowFee) =>
  Protocol.makeTransaction(
    ~source,
    ~transfers=
      buildTransfers(
        inputTransfers,
        FormUtils.keepStrictXTZ,
        accounts,
        Protocol.makeTransfer(~parameter=?None, ~entrypoint=?None),
      ),
    ~forceLowFee?,
    (),
  )
  ->ProtocolTransaction;

let buildTransaction =
    (batch: list((validState, bool)), token: option(Token.t), accounts) => {
  switch (batch) {
  | [] => assert(false)
  | [(first, _), ..._] as inputTransfers =>
    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;

    switch (token) {
    | Some(token) =>
      buildTokenTransfer(inputTransfers, token, accounts, source, forceLowFee)
    | None =>
      buildProtocolTransaction(inputTransfers, accounts, source, forceLowFee)
    };
  };
};
