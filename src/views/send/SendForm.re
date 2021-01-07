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
  let mapIfAdvanced = (v, sim, map) =>
    advancedOptionOpened && v->Js.String2.length > 0
      ? sim == Some(map(v)) ? None : Some(v->map) : None;

  let dryRun: option(Protocol.simulationResults) = values.dryRun;
  let dryRunMap = f => dryRun->Belt.Option.map(f);

  let source = values.sender;
  let amount = values.amount->Js.Float.fromString;
  let destination = values.recipient;
  let fee =
    values.fee->mapIfAdvanced(dryRunMap(d => d.fee), Js.Float.fromString);
  let counter =
    values.counter->mapIfAdvanced(dryRunMap(d => d.count), int_of_string);
  let gasLimit =
    values.gasLimit->mapIfAdvanced(dryRunMap(d => d.gasLimit), int_of_string);
  let storageLimit =
    values.storageLimit
    ->mapIfAdvanced(dryRunMap(d => d.storageLimit), int_of_string);
  let forceLowFee =
    advancedOptionOpened && values.forceLowFee ? Some(true) : None;

  switch (token) {
  | Some(token) =>
    let amount = amount->int_of_float;
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
  | [(first, _), ..._] as transfers =>
    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;

    let counter =
      first.counter->Js.String2.length > 0
        ? Some(first.counter->int_of_string) : None;

    let transfers =
      transfers->Belt.List.map(((t: StateLenses.state, advOpened)) => {
        let mapIfAdvanced = (v, map) =>
          advOpened && v->Js.String2.length > 0 ? Some(v->map) : None;

        let amount = t.amount->Js.Float.fromString;
        let destination = t.recipient;
        let gasLimit = t.gasLimit->mapIfAdvanced(int_of_string);
        let storageLimit = t.storageLimit->mapIfAdvanced(int_of_string);
        let fee = t.fee->mapIfAdvanced(Js.Float.fromString);
        Protocol.makeTransfer(
          ~amount,
          ~destination,
          ~fee?,
          ~gasLimit?,
          ~storageLimit?,
          (),
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
