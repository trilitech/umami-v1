open Protocol;
type t =
  | Protocol(Protocol.t)
  | Token(Token.operation);

let transfer = t => t->Transfer->Protocol;
let delegation = d => d->Delegation->Protocol;

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ~counter=?, ()) => {
  {
    source,
    delegate,
    options:
      Protocol.makeCommonOptions(
        ~fee,
        ~burnCap,
        ~forceLowFee,
        ~counter,
        ~confirmations=None,
        (),
      ),
  }
  ->Delegation
  ->Protocol;
};

let makeTransfer =
    (
      ~source,
      ~amount,
      ~destination,
      ~fee=?,
      ~counter=?,
      ~gasLimit=?,
      ~storageLimit=?,
      ~burnCap=?,
      ~confirmations=?,
      ~forceLowFee=?,
      (),
    ) => {
  Protocol.{
    source,
    amount,
    destination,
    tx_options:
      makeTransferOptions(
        ~fee,
        ~gasLimit,
        ~storageLimit,
        ~parameter=None,
        ~entrypoint=None,
        (),
      ),
    common_options:
      Protocol.makeCommonOptions(
        ~fee,
        ~counter,
        ~burnCap,
        ~confirmations,
        ~forceLowFee,
        (),
      ),
  }
  ->transfer;
};

module Business = {
  module Reveal = {
    type t = {public_key: string};

    let decode = json =>
      Json.Decode.{public_key: json |> field("public_key", string)};
  };

  module Transaction = {
    type t = {
      amount: string,
      destination: string,
      parameters: option(Js.Dict.t(string)),
    };

    let decode = json =>
      Json.Decode.{
        amount: json |> field("amount", string),
        destination: json |> field("destination", string),
        parameters: json |> optional(field("parameters", dict(string))),
      };
  };

  module Origination = {
    type t = {
      delegate: option(string),
      contract_address: string,
    };

    let decode = json =>
      Json.Decode.{
        delegate: json |> optional(field("delegate", string)),
        contract_address: json |> field("contract_address", string),
      };
  };

  module Delegation = {
    type t = {delegate: option(string)};

    let decode = json =>
      Json.Decode.{
        delegate:
          switch (json |> optional(field("delegate", string))) {
          | Some(delegate) =>
            delegate->Js.String2.length == 0 ? None : Some(delegate)
          | None => None
          },
      };
  };

  type payload =
    | Reveal(Reveal.t)
    | Transaction(Transaction.t)
    | Origination(Origination.t)
    | Delegation(Delegation.t)
    | Unknown;

  type t = {
    source: string,
    fee: string,
    op_id: int,
    payload,
  };

  let decode = (~typ=?, ~op_id=?, json) => {
    open Json.Decode;

    let def = (v, f) =>
      switch (v) {
      | None => f()
      | Some(v) => v
      };

    let typ = typ->def(() => json |> field("type", string));
    let op_id = op_id->def(() => json |> field("op_id", int));

    let x = {
      source: json |> field("source", string),
      fee: json |> field("fee", string),
      op_id,
      payload:
        switch (typ) {
        | "reveal" => Reveal(json->Reveal.decode)
        | "transaction" => Transaction(json->Transaction.decode)
        | "origination" => Origination(json->Origination.decode)
        | "delegation" => Delegation(json->Delegation.decode)
        | _ => Unknown
        },
    };
    x;
  };
};

module Read = {
  type payload =
    | Business(Business.t);

  type status =
    | Mempool
    | Chain;

  type t = {
    id: string,
    level: string,
    timestamp: Js.Date.t,
    block: option(string),
    hash: string,
    status,
    payload,
  };

  type operation = t;

  let decode = json => {
    Json.Decode.{
      id: json |> field("id", string),
      level: json |> field("level", string),
      timestamp: json |> field("timestamp", date),
      block: json |> field("block", optional(string)),
      hash: json |> field("hash", string),
      payload: Business(Business.decode(json)),
      status: Chain,
    };
  };

  let decodeFromMempool = json => {
    open Json.Decode;
    let typ = json |> field("operation_kind", string);
    let op_id = json |> field("id", int);
    let op =
      json |> field("operation", Js.Json.stringify) |> Json.parseOrRaise;
    {
      id: op_id |> string_of_int,
      level: json |> field("last_seen_level", int) |> string_of_int,
      timestamp:
        json
        |> field("first_seen_timestamp", float)
        |> ( *. )(1000.)
        |> Js.Date.fromFloat,
      block: json |> optional(field("block", string)),
      hash: json |> field("ophash", string),
      payload: Business(Business.decode(~typ, ~op_id, op)),
      status: Mempool,
    };
  };

  module Comparator =
    Belt.Id.MakeComparable({
      type t = operation;
      let cmp = ({hash: hash1}, {hash: hash2}) =>
        Pervasives.compare(hash1, hash2);
    });
};
