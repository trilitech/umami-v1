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
  | TokenTransfer(Token.operation, int, string, string);

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
  let dryRunMap = f => dryRun->Belt.Option.map(f);
  let source = values.sender;
  let destination = values.recipient;
  let fee =
    values.fee->mapIfAdvanced(dryRunMap(d => d.fee), ProtocolXTZ.fromString);
  let counter =
    values.counter
    ->mapIfAdvanced(dryRunMap(d => d.count), Belt.Int.fromString);
  let gasLimit =
    values.gasLimit
    ->mapIfAdvanced(dryRunMap(d => d.gasLimit), Belt.Int.fromString);
  let storageLimit =
    values.storageLimit
    ->mapIfAdvanced(dryRunMap(d => d.storageLimit), Belt.Int.fromString);
  let forceLowFee =
    advancedOptionOpened && values.forceLowFee ? Some(true) : None;

  switch (token) {
  | Some(token) =>
    let amount = values.amount->int_of_string;
    let transfer =
      Token.makeTransfer(
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
    TokenTransfer(transfer, amount, source, destination);
  | None =>
    let amount =
      values.amount
      ->ProtocolXTZ.fromString
      ->Belt.Option.getWithDefault(ProtocolXTZ.zero);
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
  | TokenTransfer(operation, _, _, _) => Operation.Token(operation)
  };

let toSimulation = (~index=?, t: transaction) =>
  switch (t) {
  | ProtocolTransaction(transaction) =>
    Operation.Simulation.transaction(transaction, index)
  | TokenTransfer(operation, _, _, _) =>
    Operation.Simulation.Token(operation)
  };

let buildTransaction =
    (batch: list((StateLenses.state, bool)), token: option(Token.t)) => {
  switch (batch) {
  | [] => assert(false)
  | [(transfer, advOpened)] => buildTransfer(transfer, advOpened, token)
  | [(first, _), ..._] as inputTransfers =>
    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;

    let counter =
      first.counter->Js.String2.length > 0
        ? Some(first.counter->int_of_string) : None;

    let transfers =
      inputTransfers->Belt.List.keepMap(((t: StateLenses.state, advOpened)) => {
        let mapIfAdvanced = (v, flatMap) =>
          advOpened && v->Js.String2.length > 0 ? v->flatMap : None;

        let amount = t.amount->ProtocolXTZ.fromString;
        let destination = t.recipient;
        let gasLimit = t.gasLimit->mapIfAdvanced(Belt.Int.fromString);
        let storageLimit = t.storageLimit->mapIfAdvanced(Belt.Int.fromString);
        let fee = t.fee->mapIfAdvanced(ProtocolXTZ.fromString);
        amount->Belt.Option.map(amount =>
          Protocol.makeTransfer(
            ~amount,
            ~destination,
            ~fee?,
            ~gasLimit?,
            ~storageLimit?,
            (),
          )
        );
      });

    Protocol.makeTransaction(
      ~source,
      ~transfers,
      ~counter?,
      ~forceLowFee?,
      (),
    )
    ->ProtocolTransaction;
  };
};
