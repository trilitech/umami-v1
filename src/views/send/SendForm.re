type amount =
  | XTZ(ProtocolXTZ.t)
  | Token(int);

module StateLenses = [%lenses
  type state = {
    amount: string,
    sender: string,
    recipient: string,
    fee: string,
    counter: string,
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
    Operation.Simulation.Token(transfer->Token.transfer)
  };

let buildTransfers = (transfers, parseAmount, build) => {
  transfers->List.keepMap(((t: StateLenses.state, advOpened)) => {
    let mapIfAdvanced = (v, flatMap) =>
      advOpened && v->Js.String2.length > 0 ? v->flatMap : None;

    let destination = t.recipient;
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
    (inputTransfers, token: Token.t, source, counter, forceLowFee) =>
  TokenTransfer(
    Token.makeTransfers(
      ~source,
      ~transfers=
        buildTransfers(
          inputTransfers,
          Int.fromString,
          Token.makeSingleTransferElt(~token=token.address),
        ),
      ~counter?,
      ~forceLowFee?,
      (),
    ),
    token,
  );

let buildProtocolTransaction = (inputTransfers, source, counter, forceLowFee) =>
  Protocol.makeTransaction(
    ~source,
    ~transfers=
      buildTransfers(
        inputTransfers,
        ProtocolXTZ.fromString,
        Protocol.makeTransfer(~parameter=?None, ~entrypoint=?None),
      ),
    ~counter?,
    ~forceLowFee?,
    (),
  )
  ->ProtocolTransaction;

let buildTransaction =
    (batch: list((StateLenses.state, bool)), token: option(Token.t)) => {
  switch (batch) {
  | [] => assert(false)
  | [(first, _), ..._] as inputTransfers =>
    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;

    let counter =
      first.counter->Js.String2.length > 0
        ? Some(first.counter->int_of_string) : None;

    switch (token) {
    | Some(token) =>
      buildTokenTransfer(inputTransfers, token, source, counter, forceLowFee)
    | None =>
      buildProtocolTransaction(inputTransfers, source, counter, forceLowFee)
    };
  };
};
