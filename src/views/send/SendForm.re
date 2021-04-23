type amount =
  | XTZ(ProtocolXTZ.t)
  | Token(int);

module StateLenses = [%lenses
  type state = {
    amount: string,
    sender: string,
    recipient: string,
    fee: string,
    gasLimit: string,
    storageLimit: string,
    forceLowFee: bool,
    dryRun: option(Protocol.simulationResults),
  }
];

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
  transfers->List.keepMap(((t: StateLenses.state, advOpened)) => {
    let mapIfAdvanced = (v, flatMap) =>
      advOpened && v->Js.String2.length > 0 ? v->flatMap : None;

    let destination = resolveAlias(accounts, t.recipient);

    let gasLimit = t.gasLimit->mapIfAdvanced(Int.fromString);
    let storageLimit = t.storageLimit->mapIfAdvanced(Int.fromString);
    let fee = t.fee->mapIfAdvanced(ProtocolXTZ.fromString);

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
          Token.Repr.fromNatString,
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
        ProtocolXTZ.fromString,
        accounts,
        Protocol.makeTransfer(~parameter=?None, ~entrypoint=?None),
      ),
    ~forceLowFee?,
    (),
  )
  ->ProtocolTransaction;

let buildTransaction =
    (
      batch: list((StateLenses.state, bool)),
      token: option(Token.t),
      accounts,
    ) => {
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
