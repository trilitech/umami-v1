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
      parameters: option(string),
    };

    let decode = json =>
      Json.Decode.{
        amount: json |> field("amount", string),
        destination: json |> field("destination", string),
        parameters: json |> optional(field("parameters", string)),
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
      Json.Decode.{delegate: json |> optional(field("delegate", string))};
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

  let decode = json => {
    let x =
      Json.Decode.{
        source: json |> field("source", string),
        fee: json |> field("fee", string),
        op_id: json |> field("op_id", int),
        payload:
          switch (json |> field("type", string)) {
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

type payload =
  | Business(Business.t);

type t = {
  id: string,
  level: string,
  timestamp: Js.Date.t,
  block: string,
  hash: string,
  payload,
};
/*
 amount:
       json
       |> field("amount", string)
       |> Js.Float.fromString
       |> (x => x /. 1000000.),
 */

let decode = json =>
  Json.Decode.{
    id: json |> field("id", string),
    level: json |> field("level", string),
    timestamp: json |> field("timestamp", date),
    block: json |> field("block", string),
    hash: json |> field("hash", string),
    payload: Business(json->Business.decode),
  };