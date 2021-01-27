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
  | TokenTransfer(Token.t, Token.Transfer.t);

let buildTransfer =
    (
      values: StateLenses.state,
      advancedOptionOpened: bool,
      token: option(Token.t),
    ) => {
  let mapIfAdvanced = (v, sim, flatMap) =>
    advancedOptionOpened && v->Js.String2.length > 0
      ? sim == flatMap(v) ? None : v->flatMap : None;

  let dryRun: option(Protocol.simulationResults) = values.dryRun;
  let dryRunMap = f => dryRun->Option.map(f);
  let source = values.sender;
  let destination = values.recipient;
  let fee =
    values.fee->mapIfAdvanced(dryRunMap(d => d.fee), ProtocolXTZ.fromString);
  let counter =
    values.counter->mapIfAdvanced(dryRunMap(d => d.count), Int.fromString);
  let gasLimit =
    values.gasLimit
    ->mapIfAdvanced(dryRunMap(d => d.gasLimit), Int.fromString);
  let storageLimit =
    values.storageLimit
    ->mapIfAdvanced(dryRunMap(d => d.storageLimit), Int.fromString);
  let forceLowFee =
    advancedOptionOpened && values.forceLowFee ? Some(true) : None;

  switch (token) {
  | Some(token) =>
    let amount = values.amount->int_of_string;
    let transfer =
      Token.makeSingleTransfer(
        ~source,
        ~amount,
        ~destination,
        ~contract=token.address,
        ~fee?,
        ~counter?,
        ~gasLimit?,
        ~storageLimit?,
        ~forceLowFee?,
        (),
      );
    TokenTransfer(token, transfer);
  | None =>
    let amount =
      values.amount
      ->ProtocolXTZ.fromString
      ->Option.getWithDefault(ProtocolXTZ.zero);
    let t =
      Protocol.makeSingleTransaction(
        ~source,
        ~amount,
        ~destination,
        ~fee?,
        ~counter?,
        ~gasLimit?,
        ~storageLimit?,
        ~forceLowFee?,
        (),
      );
    ProtocolTransaction(t);
  };
};

let toOperation = (t: transaction) =>
  switch (t) {
  | ProtocolTransaction(transaction) => Operation.transaction(transaction)
  | TokenTransfer(_, transfer) => Operation.Token(transfer->Token.transfer)
  };

let toSimulation = (~index=?, t: transaction) =>
  switch (t) {
  | ProtocolTransaction(transaction) =>
    Operation.Simulation.transaction(transaction, index)
  | TokenTransfer(_, transfer) =>
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

let buildTransaction =
    (batch: list((StateLenses.state, bool)), token: option(Token.t)) => {
  switch (batch) {
  | [] => assert(false)
  | [(transfer, advOpened)] => buildTransfer(transfer, advOpened, None)
  | [(first, _), ..._] as inputTransfers =>
    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;

    let counter =
      first.counter->Js.String2.length > 0
        ? Some(first.counter->int_of_string) : None;

    switch (token) {
    | Some(token) =>
      TokenTransfer(
        token,
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
      )
    | None =>
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
      ->ProtocolTransaction
    };
  };
};
