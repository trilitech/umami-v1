/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

module Reveal = {
  type t = {public_key: string}

  open Json.Decode

  let decode = json => {
    let pk = json |> optional(field("data", field("public_key", string)))
    pk->Option.map(public_key => {public_key: public_key})
  }
}

module Transaction = {
  type common = {
    amount: Tez.t,
    destination: PublicKeyHash.t,
    parameters: option<Js.Dict.t<string>>,
    entrypoint: option<string>,
  }
  type token_info = {
    kind: TokenRepr.kind,
    amount: TokenRepr.Unit.t,
    contract: PublicKeyHash.t,
  }

  type internal_op_id = int

  type t =
    | Tez(common)
    | Token(common, token_info, option<internal_op_id>)

  type tokenKind = TokenContract.kind

  type kind = [tokenKind | #KTez]

  module Accessor = {
    let destination = x =>
      switch x {
      | Token({destination, _}, _, _)
      | Tez({destination, _}) => destination
      }
  }

  module Decode = {
    open Json.Decode

    let kindFromString = k =>
      switch TokenContract.Decode.kindFromString(k) {
      | Error(TokenContract.UnknownKind("tez")) => Ok(#KTez)
      // Ok(k) must be destructed and rebuilt to allow the unification of
      // TokenContract.kind with `KTez
      | Ok(k) => Ok((k :> kind))
      | Error(e) => Error(e)
      }

    let token_kind = json =>
      (json |> field("data", field("token", string)))->kindFromString->Result.getExn

    let token_id = json =>
      (json |> field("data", field("token_id", string)))->int_of_string_opt->Option.getExn

    let internal_op_id = json => json |> field("data", optional(field("internal_op_id", int)))

    let token_info = (json, kind) => {
      let kind = switch kind {
      | #KFA1_2 => TokenRepr.FA1_2
      | #KFA2 => TokenRepr.FA2(token_id(json))
      }
      {
        kind: kind,
        amount: json
        |> field("data", field("token_amount", string))
        |> TokenRepr.Unit.fromNatString
        |> Result.getExn,
        contract: json
        |> field("data", field("contract", string))
        |> PublicKeyHash.build
        |> Result.getExn,
      }
    }

    let common = json => {
      amount: json |> field("data", field("amount", string)) |> Tez.fromMutezString,
      destination: json
      |> field("data", field("destination", string))
      |> PublicKeyHash.build
      |> Result.getExn,
      parameters: json |> optional(field("parameters", dict(string))),
      entrypoint: json |> field("data", JsonEx.Decode.optionalOrNull("entrypoint", string)),
    }

    let t = json => {
      let token = json->token_kind
      switch token {
      | #...tokenKind as kind => Token(common(json), token_info(json, kind), internal_op_id(json))
      | #KTez => Tez(common(json))
      }
    }

    module Tzkt = {
      let t = json => {
        let common = json => {
          amount: json |> field("amount", int) |> Tez.fromMutezInt,
          destination: json
          |> field("target", field("address", string))
          |> PublicKeyHash.build
          |> Result.getExn,
          entrypoint: json |> optional(
            field("parameter", json =>
              Option.getWithDefault(json |> optional(field("entrypoint", string)), "default")
            ),
          ),
          parameters: json |> optional(field("parameter", dict(string))),
        }
        json |> optional(field("tokenTransfersCount", int)) == None ? Some(Tez(common(json))) : None
      }
    }
  }
}

module Origination = {
  type t = {contract: string}

  module Decode = {
    open Json.Decode

    let t = json => {
      let ca = json |> optional(field("data", field("contract", string)))

      ca->Option.map(contract => {contract: contract})
    }

    module Tzkt = {
      let t = json => {
        let ca = json |> optional(field("originatedContract", optional(field("address", string))))

        ca->Option.flatMap(x => x->Option.map(x => {contract: x}))
      }
    }
  }
}

module Delegation = {
  type t = {delegate: option<PublicKeyHash.t>}

  module Decode = {
    open Json.Decode

    let t = json => {
      {
        delegate: switch json |> optional(field("data", field("delegate", string))) {
        | Some(delegate) =>
          delegate->Js.String2.length == 0
            ? None
            : Some(delegate->PublicKeyHash.build->Result.getExn)
        | None => None
        },
      }
    }

    module Tzkt = {
      let t = json => {
        {
          delegate: json
          |> optional(field("newDelegate", field("address", string)))
          |> (
            delegate => switch delegate {
              | None
              | Some("") => None
              | Some(delegate) => Some(delegate->PublicKeyHash.build->Result.getExn)
            }
          ),
        }
      }
    }
  }
}

type payload =
  | Reveal(option<Reveal.t>)
  | Transaction(Transaction.t)
  | Origination(option<Origination.t>)
  | Delegation(Delegation.t)
  | Unknown

type status =
  | Mempool
  | Chain

type t = {
  block: option<string>,
  fee: Tez.t,
  hash: string,
  id: string, // FIXME: make it an integer
  level: int,
  op_id: int, // FIXME: remove this one, which is juste `id` converted to int
  payload: payload,
  source: PublicKeyHash.t,
  status: status,
  timestamp: Js.Date.t,
  internal: int,
}

let internal_op_id = op =>
  switch op.payload {
  | Transaction(Token(_, _, internal_op_id)) => internal_op_id
  | _ => None
  }

let uniqueId = op => (op.hash, op.id, op.internal, op->internal_op_id)
let uniqueIdToString = ((hash, id, iid, iiid)) =>
  hash ++ id ++ iid->Int.toString ++ iiid->Option.mapWithDefault("", Int.toString)

type operation = t

module Decode = {
  open Json.Decode

  let payload = (ty, json) =>
    switch ty {
    | "reveal" => Reveal(json->Reveal.decode)
    | "transaction" => Transaction(json->Transaction.Decode.t)
    | "origination" => Origination(json->Origination.Decode.t)
    | "delegation" => Delegation(json->Delegation.Decode.t)
    | _ => Unknown
    }

  let source = json => json |> field("src", string) |> PublicKeyHash.build |> Result.getExn

  let status = json => {
    let block_hash = json |> optional(field("block_hash", string))
    Option.isNone(block_hash) ? Mempool : Chain
  }

  let internal = json => (json |> optional(field("internal", int)))->Option.getWithDefault(0)

  let t = json => {
    let internal = json |> internal
    {
      block: json |> optional(field("block_hash", string)),
      fee: internal == 0 ? (json |> optional(field("fee", string)))->Option.mapWithDefault(Tez.zero, Tez.fromMutezString) : Tez.zero,
      hash: json |> field("hash", string),
      id: json |> field("id", string),
      level: json |> field("level", string) |> int_of_string,
      op_id: json |> field("id", string) |> int_of_string,
      payload: json |> payload(json |> field("kind", string)),
      source: json |> source,
      status: status(json),
      timestamp: json |> field("op_timestamp", date),
      internal: internal,
    }
  }

  module Tzkt = {
    let payload = (ty, json) =>
      switch ty {
      | "reveal" => None
      | "transaction" => Option.map(json->Transaction.Decode.Tzkt.t, t => Transaction(t))
      | "origination" => json->Origination.Decode.Tzkt.t->Origination->Some
      | "delegation" => json->Delegation.Decode.Tzkt.t->Delegation->Some
      | _ => None
      }

    let t = json =>
      Option.map(json |> payload(json |> field("type", string)), payload => {
        block: json |> optional(field("block", string)),
        fee: json |> field("bakerFee", int) |> Tez.fromMutezInt, // Mezos does not use storageFee or allocationFee
        hash: json |> field("hash", string),
        id: json |> field("id", int) |> Int.toString,
        level: json |> field("level", int),
        op_id: json |> field("id", int),
        payload: payload,
        source: json
        |> field("sender", field("address", string))
        |> PublicKeyHash.build
        |> Result.getExn,
        status: json |> field("status", string) == "applied" ? Chain : Mempool,
        timestamp: json |> field("timestamp", date),
        internal: json |> optional(field("initiator", Json.Decode.id)) == None ? 0 : 1, // Can't have a real position, only true or false
      })
  }
}

module Comparator = Id.MakeComparable({
  type t = operation
  let cmp = (op1, op2) => Pervasives.compare(uniqueId(op1), uniqueId(op2))
})
