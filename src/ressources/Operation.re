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
  | Transfer(Transfer.t);

let transaction = t => t->Transaction->Protocol;
let delegation = d => d->Delegation->Protocol;
let transfer = b => b->Transfer;

module Simulation = {
  type index = option(int);

  type t =
    | Protocol(Protocol.t, index)
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
module Reveal = {
  type t = {public_key: string};

  let decode = json =>
    Json.Decode.{
      public_key: json |> field("data", field("public_key", string)),
    };
};

module Transaction = {
  type common = {
    amount: Tez.t,
    destination: PublicKeyHash.t,
    parameters: option(Js.Dict.t(string)),
  };
  type token_info = {
    amount: TokenRepr.Unit.t,
    contract: PublicKeyHash.t,
  };
  type t =
    | Tez(common)
    | Token(common, token_info);

  module Accessor = {
    let amount =
      fun
      | Token({amount, _}, _)
      | Tez({amount, _}) => amount;

    let destination =
      fun
      | Token({destination, _}, _)
      | Tez({destination, _}) => destination;
  };

  module Decode = {
    open Json.Decode;

    let token_info = json => {
      amount:
        json
        |> field("data", field("token_amount", string))
        |> TokenRepr.Unit.fromNatString
        |> Option.getExn,
      contract:
        json
        |> field("data", field("contract", string))
        |> PublicKeyHash.build
        |> Result.getExn,
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

    // FIXME: This exists due to a bug in mezos, where fa1.2 operations
    // are badly queried when they're in the mempool. Thus, we simply
    // print them as a normal contract call
    let fa12_mempool = json => {
      amount:
        json |> field("data", field("amount", string)) |> Tez.fromMutezString,
      destination:
        json
        |> field("data", field("contract", string))
        |> PublicKeyHash.build
        |> Result.getExn,
      parameters: json |> optional(field("parameters", dict(string))),
    };

    let t = json => {
      let token = json |> field("data", field("token", bool));
      let is_fa12_mempool =
        json
        |> field("data", optional(field("destination", string)))
        |> Option.isNone;
      switch (token, is_fa12_mempool) {
      | (true, true) => Tez(fa12_mempool(json))
      | (true, _) => Token(common(json), token_info(json))
      | (_, _) => Tez(common(json))
      };
    };
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
    | Reveal(Reveal.t)
    | Transaction(Transaction.t)
    | Origination(Origination.t)
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
      let cmp = ({hash: hash1, op_id: id1}, {hash: hash2, op_id: id2}) =>
        Pervasives.compare((hash1, id1), (hash2, id2));
    });
};
