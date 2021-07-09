/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ProtocolOptions;
open Protocol;

type t =
  | Protocol(Protocol.t)
  | Token(Token.operation)
  | Transfer(Transfer.t);

let transaction = t => t->Transaction->Protocol;
let delegation = d => d->Delegation->Protocol;
let transfer = b => b->Transfer;

module Simulation = {
  type index = option(int);

  type t =
    | Protocol(Protocol.t, index)
    | Token(Token.operation, index)
    | Transfer(Transfer.t, index);

  let delegation = d => Protocol(d->Delegation, None);
  let transaction = (t, index) => Protocol(t->Transaction, index);
  let batch = (b, index) => Transfer(b, index);
};

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  {
    source,
    delegate,
    options: makeCommonOptions(~fee, ~burnCap, ~forceLowFee, ()),
  }
  ->delegation;
};

let makeTransaction =
    (~source, ~transfers, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) =>
  transaction(
    Transfer.makeTransfers(
      ~source,
      ~transfers,
      ~fee?,
      ~burnCap?,
      ~forceLowFee?,
      (),
    ),
  );

let makeSingleTransaction =
    (
      ~source,
      ~amount,
      ~destination,
      ~burnCap=?,
      ~forceLowFee=?,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) => {
  let transfers = [
    Transfer.makeSingleTezTransferElt(
      ~amount,
      ~destination,
      ~fee?,
      ~parameter?,
      ~entrypoint?,
      ~gasLimit?,
      ~storageLimit?,
      (),
    ),
  ];

  makeTransaction(~source, ~transfers, ~fee?, ~burnCap?, ~forceLowFee?, ());
};

module Business = {
  module Reveal = {
    type t = {public_key: string};

    let decode = json =>
      Json.Decode.{public_key: json |> field("public_key", string)};
  };

  module Transaction = {
    type t = {
      amount: Tez.t,
      destination: PublicKeyHash.t,
      parameters: option(Js.Dict.t(string)),
    };

    let decode = json =>
      Json.Decode.{
        amount: json |> field("amount", string) |> Tez.fromMutezString,
        destination:
          json
          |> field("destination", string)
          |> PublicKeyHash.build
          |> Result.getExn,
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
    type t = {delegate: option(PublicKeyHash.t)};

    let decode = json =>
      Json.Decode.{
        delegate:
          switch (json |> optional(field("delegate", string))) {
          | Some(delegate) =>
            delegate->Js.String2.length == 0
              ? None : Some(delegate->PublicKeyHash.build->Result.getExn)
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
    source: PublicKeyHash.t,
    fee: Tez.t,
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
      source:
        json
        |> field("source", string)
        |> PublicKeyHash.build
        |> Result.getExn,
      fee: json |> field("fee", string) |> Tez.fromMutezString,
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
    op_id: int,
    level: int,
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
      op_id: json |> field("op_id", int),
      level: json |> field("level", string) |> int_of_string,
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
    let op_id = json |> field("id", string) |> int_of_string;
    let op =
      json |> field("operation", Js.Json.stringify) |> Json.parseOrRaise;
    {
      id: "",
      op_id,
      level: json |> field("last_seen_level", int),
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
    Id.MakeComparable({
      type t = operation;
      let cmp = ({hash: hash1, op_id: id1}, {hash: hash2, op_id: id2}) =>
        Pervasives.compare((hash1, id1), (hash2, id2));
    });
};
