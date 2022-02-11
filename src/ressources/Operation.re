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

type t = Protocol.t;

let transaction = t => t->Transaction;
let origination = t => t->Origination;
let delegation = d => d->Delegation;

module Simulation = {
  type t = Protocol.t;

  let delegation = d => d->Delegation;
  let origination = o => o->Origination;
  let transaction = t => t->Transaction;
};

let makeDelegate =
    (~source, ~delegate, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  {
    source,
    delegate,
    options: makeDelegationOptions(~fee, ~burnCap, ~forceLowFee, ()),
  }
  ->delegation;
};

let makeOrigination =
    (
      ~source,
      ~balance=?,
      ~code,
      ~storage,
      ~delegate=?,
      ~fee=?,
      ~burnCap=?,
      ~forceLowFee=?,
      (),
    ) =>
  {
    source,
    balance,
    code,
    storage,
    delegate,
    options: makeOriginationOptions(~fee, ~burnCap, ~forceLowFee, ()),
  }
  ->origination;

let makeTransaction = (~source, ~transfers, ~burnCap=?, ~forceLowFee=?, ()) =>
  transaction(
    Transfer.makeTransfers(~source, ~transfers, ~burnCap?, ~forceLowFee?, ()),
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

  makeTransaction(~source, ~transfers, ~burnCap?, ~forceLowFee?, ());
};
module Reveal = {
  type t = {public_key: string};

  open Json.Decode;

  let decode = json => {
    let pk = json |> optional(field("data", field("public_key", string)));
    pk->Option.map(public_key => {public_key: public_key});
  };
};

module Transaction = {
  type common = {
    amount: Tez.t,
    destination: PublicKeyHash.t,
    parameters: option(Js.Dict.t(string)),
  };
  type token_info = {
    kind: TokenRepr.kind,
    amount: TokenRepr.Unit.t,
    contract: PublicKeyHash.t,
  };

  type internal_op_id = int;

  type t =
    | Tez(common)
    | Token(common, token_info, option(internal_op_id));

  type tokenKind = TokenContract.kind;

  type kind = [ tokenKind | `KTez];

  module Accessor = {
    let amount =
      fun
      | Token({amount, _}, _, _)
      | Tez({amount, _}) => amount;

    let destination =
      fun
      | Token({destination, _}, _, _)
      | Tez({destination, _}) => destination;
  };

  module Decode = {
    open Json.Decode;

    let kindFromString = k =>
      switch (TokenContract.Decode.kindFromString(k)) {
      | Error(TokenContract.UnknownKind("tez")) => Ok(`KTez)
      // Ok(k) must be destructed and rebuilt to allow the unification of
      // TokenContract.kind with `KTez
      | Ok(k) => Ok((k :> kind))
      | Error(e) => Error(e)
      };

    let token_kind = json =>
      (json |> field("data", field("token", string)))
      ->kindFromString
      ->Result.getExn;

    let token_id = json =>
      (json |> field("data", field("token_id", string)))
      ->int_of_string_opt
      ->Option.getExn;

    let internal_op_id = json => {
      json |> field("data", optional(field("internal_op_id", int)));
    };

    let token_info = (json, kind) => {
      let kind =
        switch (kind) {
        | `KFA1_2 => TokenRepr.FA1_2
        | `KFA2 => TokenRepr.FA2(token_id(json))
        };
      {
        kind,
        amount:
          json
          |> field("data", field("token_amount", string))
          |> TokenRepr.Unit.fromNatString
          |> Result.getExn,
        contract:
          json
          |> field("data", field("contract", string))
          |> PublicKeyHash.build
          |> Result.getExn,
      };
    };

    let common = json => {
      amount:
        json |> field("data", field("amount", string)) |> Tez.fromMutezString,
      destination:
        json
        |> field("data", field("destination", string))
        |> PublicKeyHash.build
        |> Result.getExn,
      parameters: json |> optional(field("parameters", dict(string))),
    };

    let t = json => {
      let token = json->token_kind;
      switch (token) {
      | #tokenKind as kind =>
        Token(common(json), token_info(json, kind), internal_op_id(json))
      | `KTez => Tez(common(json))
      };
    };
  };
};

module Origination = {
  type t = {contract: string};

  open Json.Decode;

  let decode = json => {
    let ca = json |> optional(field("data", field("contract", string)));

    ca->Option.map(contract => {contract: contract});
  };
};

module Delegation = {
  type t = {delegate: option(PublicKeyHash.t)};

  let decode = json =>
    Json.Decode.{
      delegate:
        switch (json |> optional(field("data", field("delegate", string)))) {
        | Some(delegate) =>
          delegate->Js.String2.length == 0
            ? None : Some(delegate->PublicKeyHash.build->Result.getExn)
        | None => None
        },
    };
};

module Read = {
  type payload =
    | Reveal(option(Reveal.t))
    | Transaction(Transaction.t)
    | Origination(option(Origination.t))
    | Delegation(Delegation.t)
    | Unknown;

  type status =
    | Mempool
    | Chain;

  type t = {
    block: option(string),
    fee: Tez.t,
    hash: string,
    id: string,
    level: int,
    op_id: int,
    payload,
    source: PublicKeyHash.t,
    status,
    timestamp: Js.Date.t,
  };

  let filterJsonExn = ex =>
    switch (ex) {
    | Json.ParseError(error) => error
    | Json.Decode.DecodeError(error) => error
    | _ => "Unknown error"
    };

  let internal_op_id = op =>
    switch (op.payload) {
    | Transaction(Token(_, _, internal_op_id)) => internal_op_id
    | _ => None
    };

  let uniqueId = op => (op.hash, op.id, internal_op_id(op));
  let uniqueIdToString = ((hash, id, iid)) =>
    hash ++ id ++ iid->Option.mapWithDefault("", Int.toString);

  type operation = t;

  module Decode = {
    open Json.Decode;

    let payload = (ty, json) =>
      switch (ty) {
      | "reveal" => Reveal(json->Reveal.decode)
      | "transaction" => Transaction(json->Transaction.Decode.t)
      | "origination" => Origination(json->Origination.decode)
      | "delegation" => Delegation(json->Delegation.decode)
      | _ => Unknown
      };

    let source = json =>
      json |> field("src", string) |> PublicKeyHash.build |> Result.getExn;

    let status = json => {
      let block_hash = json |> field("block_hash", optional(string));
      Option.isNone(block_hash) ? Mempool : Chain;
    };

    let t = json => {
      {
        block: json |> field("block_hash", optional(string)),
        fee: json |> field("fee", string) |> Tez.fromMutezString,
        hash: json |> field("hash", string),
        id: json |> field("id", string),
        level: json |> field("level", string) |> int_of_string,
        op_id: json |> field("id", string) |> int_of_string,
        payload: json |> payload(json |> field("kind", string)),
        source: json |> source,
        status: status(json),
        timestamp: json |> field("op_timestamp", date),
      };
    };
  };

  module Comparator =
    Id.MakeComparable({
      type t = operation;
      let cmp = (op1, op2) =>
        Pervasives.compare(uniqueId(op1), uniqueId(op2));
    });
};
