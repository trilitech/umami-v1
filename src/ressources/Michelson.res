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

module Decode = {
  open Json.Decode

  type address =
    | Packed(bytes)
    | Pkh(PublicKeyHash.t)

  let toPublicKeyHash = (a: address, encode): PublicKeyHash.t =>
    switch a {
    | Packed(b) => b->Bytes.unsafe_to_string->encode->PublicKeyHash.build->Result.getExn
    | Pkh(pkh) => pkh
    }

  let dataDecoder = params => field("data", array(params))

  let pairDecoder = (d1, d2) => field("args", tuple2(d1, d2))

  let intDecoder = field("int", string)

  let bytesDecoder = field("bytes", string) |> map(s => s->Bytes.unsafe_of_string)

  let stringDecoder = field("string", string)

  let addressDecoder = either(
    bytesDecoder |> map(b => Packed(b)),
    stringDecoder |> map(s => Pkh(s->PublicKeyHash.build->JsonEx.getExn)),
  )

  let fa12ParameterDecoder = json =>
    json |> pairDecoder(addressDecoder, pairDecoder(addressDecoder, intDecoder))

  let fa2ParameterDecoder = json =>
    json |> array(
      pairDecoder(
        addressDecoder,
        array(pairDecoder(addressDecoder, pairDecoder(intDecoder, intDecoder))),
      ),
    )

  /*
    Example of response for `run_view` on `balance_of` on an FA2 contract for
    a single address:
    { "data":
        [ { "prim": "Pair",
            "args":
              [ { "prim": "Pair",
                  "args":
                    [ { "bytes": "0000721765c758aacce0986e781ddc9a40f5b6b9d9c3" },
                      { "int": "0" } ] }, { "int": "1000010000" } ] } ] }

    The result is actually a Michelson list of `(pkh * tokenId * balance)`,
   but this version only parses the result for a single address.
   This version is purely adhoc for this response.
 */
  let fa2BalanceOfDecoder = json =>
    json |> dataDecoder(pairDecoder(pairDecoder(addressDecoder, intDecoder), intDecoder))
}

module MichelsonV1Expression = {
  type t = Js.Json.t
  let int = (int: string): t => {"int": int}->Obj.magic
  let string = (string: string): t => {"string": string}->Obj.magic
  let bytes = (bytes: string): t => {"bytes": bytes}->Obj.magic
  let prim = (prim: string, args: array<t>): t =>
    [] == args ? {"prim": prim}->Obj.magic : {"prim": prim, "args": args}->Obj.magic
  let seq = (array: array<t>): t => array->Obj.magic

  module Constructors = {
    type t = t
    let _Pair = (a, b) => prim("Pair", [a, b])
  }

  module Types = {
    type t = t
    let pair = (a, b) => prim("pair", [a, b])
    let list = t => prim("list", [t])
    let address = prim("address", [])
    let nat = prim("nat", [])
    let operation = prim("operation", [])
    let mutez = prim("mutez", [])
    let unit = prim("unit", [])
    let key_hash = prim("key_hash", [])
    let lambda = (t1, t2) => prim("lambda", [t1, t2])
  }
  module Instructions = {
    type t = t
    let _DROP = prim("DROP", [])
    let _NIL = t => prim("NIL", [t])
    let _PUSH = (t, v) => prim("PUSH", [t, v])
    let _CONTRACT = t => prim("CONTRACT", [t])
    let _IF_NONE = (a, b) => prim("IF_NONE", [a, b])
    let _TRANSFER_TOKENS = prim("TRANSFER_TOKENS", [])
    let _CONS = prim("CONS", [])
    let _FAILWITH = prim("FAILWITH", [])
    let _UNIT = prim("UNIT", [])
    let _SOME = prim("SOME", [])
    let _SET_DELEGATE = prim("SET_DELEGATE", [])
    let _NONE = t => prim("NONE", [t])
    let _IMPLICIT_ACCOUNT = prim("IMPLICIT_ACCOUNT", [])
    let _CREATE_CONTRACT = (parameter: Types.t, storage: Types.t, code: t) =>
      prim("CREATE_CONTRACT", [parameter, storage, code])
    let _AMOUNT = prim("AMOUNT", [])
    let _DIP = args => prim("DIP", [args])
    let _PAIR = prim("PAIR", [])
  }
}

module LAMBDA_PARSER = {
  open MichelsonV1Expression
  open Types
  open Instructions

  let check = (input, pos, value) => input[pos]->Option.mapWithDefault(false, x => x == value)
  let args = (json, name) =>
    Js.Json.decodeObject(json)->Option.flatMap(x =>
      Js_dict.get(x, "prim") == Some(Obj.magic(name)) ? Js_dict.get(x, "args") : None
    )

  // FIXME: Entrypoint my be defined in "annots" field
  // For a given <FIELD> value:
  // read {prim: "PUSH", args: [{prim: "<FIELD>"}, {bytes: <RECIPIENT>}]} and
  // return <RECIPIENT>
  let recipient = (field, encode: string => (string, string), json) => {
    json
    ->args("PUSH")
    ->Option.flatMap(Js.Json.decodeArray)
    ->Option.flatMap(x => x[0] == Some(field) ? x[1] : None)
    ->Option.flatMap(Js.Json.decodeObject)
    ->Option.flatMap(x => Js.Dict.get(x, "bytes")) // is "string" in LAMBDA_MANAGER
    ->Option.flatMap(Js.Json.decodeString)
    ->Option.map(encode)
    ->Option.flatMap(((pkh, entrypoint)) =>
      pkh->PublicKeyHash.build->ResultEx.toOption->Option.map(pkh => (pkh, entrypoint))
    )
  }

  // Extract <AMOUNT> from {prim: "PUSH", args: [{prim: "mutez"}, {int: <AMOUNT>}]}
  let amount = json =>
    json
    ->args("PUSH")
    ->Option.flatMap(Js.Json.decodeArray)
    ->Option.flatMap(x => x[0] == Some(mutez) ? x[1] : None)
    ->Option.flatMap(Js.Json.decodeObject)
    ->Option.flatMap(x => Js.Dict.get(x, "int"))
    ->Option.flatMap(Js.Json.decodeString)
    ->Option.map(Tez.fromMutezString)

  let fa12_parameter_type = pair(address, pair(address, nat))

  let fa2_parameter_type = list(pair(address, list(pair(address, pair(nat, nat)))))

  let fa12_token_amount = (json, contract, encode) =>
    Js.Json.decodeObject(json)
    ->Option.flatMap(x =>
      Js_dict.get(x, "prim") == Some(Obj.magic("PUSH")) ? Js_dict.get(x, "args") : None
    )
    ->Option.flatMap(Js.Json.decodeArray)
    ->Option.flatMap(x => x[0] == Some(fa12_parameter_type) ? x[1] : None)
    ->Option.flatMap(x => x->JsonEx.decode(Decode.fa12ParameterDecoder)->ResultEx.toOption)
    ->Option.map(((_, (to_, amount))) => {
      let amount =
        amount
        ->ReBigNumber.fromString
        ->TokenRepr.Unit.fromBigNumber
        ->ResultEx.toOption
        ->Option.getWithDefault(TokenRepr.Unit.zero)
      let destination = to_->Decode.toPublicKeyHash(encode)
      Operation.Transaction.Token(
        {
          amount: Tez.zero,
          destination: destination,
          parameters: None,
          entrypoint: None,
        },
        {
          kind: FA1_2,
          amount: amount,
          contract: contract,
        },
        None,
      )
    })
    ->Option.map(x => [x])

  let fa2_token_amount = (json, contract, encode) =>
    Js.Json.decodeObject(json)
    ->Option.flatMap(x =>
      Js_dict.get(x, "prim") == Some(Obj.magic("PUSH")) ? Js_dict.get(x, "args") : None
    )
    ->Option.flatMap(Js.Json.decodeArray)
    ->Option.flatMap(x => x[0] == Some(fa2_parameter_type) ? x[1] : None)
    ->Option.flatMap(x => x->JsonEx.decode(Decode.fa2ParameterDecoder)->ResultEx.toOption)
    ->Option.flatMap(x => x[0])
    ->Option.map(((_, recipients)) =>
      recipients->Array.map(((to_, (token_id, amount))) => {
        let destination = to_->Decode.toPublicKeyHash(encode)
        let amount =
          amount
          ->ReBigNumber.fromString
          ->TokenRepr.Unit.fromBigNumber
          ->ResultEx.toOption
          ->Option.getWithDefault(TokenRepr.Unit.zero)
        let token_id = token_id->Int.fromString->Option.getWithDefault(0)
        Operation.Transaction.Token(
          {
            amount: Tez.zero,
            destination: destination,
            parameters: None,
            entrypoint: None,
          },
          {
            kind: FA2(token_id),
            amount: amount,
            contract: contract,
          },
          None,
        )
      })
    )

  type transferParams = Operation.Transaction.t

  let transferParams = (r, rk, encode, a, input): option<transferParams> => {
    input[r]
    ->Option.flatMap(recipient(rk, encode))
    ->Option.flatMap(((recipient, entrypoint)) => {
      input[a]
      ->Option.flatMap(amount)
      ->Option.map((amount): Operation.Transaction.t => {
        Operation.Transaction.Tez({
          destination: recipient,
          entrypoint: "" == entrypoint ? None : Some(entrypoint),
          parameters: None, //FIXME
          amount: amount,
        })
      })
    })
  }

  let recipient_token_amount = (r, rk, encode, a, t, f, input): option<array<transferParams>> => {
    let encode' = s => (encode(s), "")
    input[r]
    ->Option.flatMap(recipient(rk, encode'))
    ->Option.flatMap(((recipient, _entrypoint)) => {
      input[a]
      ->Option.flatMap(amount)
      ->Option.flatMap(_ => input[t]->Option.flatMap(x => x->f(recipient, encode)))
    })
  }

  let encode_key_hash = s => (ReTaquitoUtils.encodeKeyHash(s), "")

  // See MANAGER_LAMBDA.setDelegate
  // [
  //   {prim: 'DROP'},
  //   {prim: 'NIL', args: [{ prim: 'operation' }]},
  // 0 {prim: 'PUSH', args: [{ prim: 'key_hash' }, { string: key }]},
  // 1 {prim: 'SOME'},
  // 2 {prim: 'SET_DELEGATE'},
  //   {prim: 'CONS'},
  // ]
  let setDelegate = (input: array<Js.Json.t>, start) => {
    let check = (pos, value) => check(input, pos, value)
    check(start + 1, _SOME) && check(start + 2, _SET_DELEGATE)
      ? input[start]
        ->Option.flatMap(recipient(key_hash, encode_key_hash))
        ->Option.map(((res, _)) => (res, start + 3))
      : None
  }

  // See MANAGER_LAMBDA.removeDelegate
  // [
  //   { prim: 'DROP' },
  //   { prim: 'NIL', args: [{ prim: 'operation' }] },
  // 0 { prim: 'NONE', args: [{ prim: 'key_hash' }] },
  // 1 { prim: 'SET_DELEGATE' },
  //   { prim: 'CONS' },
  // ]
  let removeDelegate = (input: array<Js.Json.t>, start) => {
    let check = (pos, value) => check(input, pos, value)
    check(start, _NONE(key_hash)) && check(start + 1, _SET_DELEGATE) ? Some(start + 2) : None
  }

  // See MANAGER_LAMBDA.transferImplicit
  // [
  //   {prim: "DROP"},
  //   {prim: "NIL", args: [{prim: "operation"}]},
  // 0 {prim: "PUSH", args: [{prim: "key_hash"}, {string: <RECIPIENT>}]},
  // 1 {prim: "IMPLICIT_ACCOUNT"},
  // 2 {prim: "PUSH", args: [{prim: "mutez"}, {int: <AMOUNT>}]},
  // 3 {prim: "UNIT"},
  // 4 {prim: "TRANSFER_TOKENS"},
  //   {prim: "CONS"},
  // ]
  let transferImplicit = (input: array<Js.Json.t>, start) => {
    let check = (pos, value) => check(input, pos, value)
    check(start + 1, _IMPLICIT_ACCOUNT) &&
    check(start + 3, _UNIT) &&
    check(start + 4, _TRANSFER_TOKENS)
      ? transferParams(start, key_hash, encode_key_hash, start + 2, input)->Option.map(res => (
          res,
          start + 5,
        ))
      : None
  }

  let encode_address = s => {
    let pkh = Js.String.slice(~from=0, ~to_=44, s)->ReTaquitoUtils.encodePubKey
    let entrypoint =
      Js.String.sliceToEnd(~from=44, s)
      ->ReTaquitoUtils.hex2buf
      ->BufferEx.toNodeBuffer
      ->Node_buffer.toString
    (pkh, entrypoint)
  }

  // See MANAGER_LAMBDA.transferToContract
  // [
  //   { prim: 'DROP' },
  //   { prim: 'NIL', args: [{ prim: 'operation' }] },
  // 0 { prim: 'PUSH', args: [{ prim: 'address' }, { string: <CONTRACT_ADDRESS> }], },
  // 1 { prim: 'CONTRACT', args: [{ prim: 'unit' }] },
  // 2 [ { prim: 'IF_NONE', args: [[[{ prim: 'UNIT' }, { prim: 'FAILWITH' }]], []] }, ],
  // 3 { prim: 'PUSH', args: [{ prim: 'mutez' }, { int: <AMOUNT> }] },
  // 4 { prim: 'UNIT' },
  // 5 { prim: 'TRANSFER_TOKENS' },
  //   { prim: 'CONS' },
  // ]
  let transferToContract = (input: array<Js.Json.t>, start) => {
    let check = (pos, value) => check(input, pos, value)
    check(start + 1, _CONTRACT(unit)) &&
    check(start + 2, seq([_IF_NONE(seq([seq([_UNIT, _FAILWITH])]), seq([]))])) &&
    check(start + 4, _UNIT) &&
    // FIXME: This is entrypoint's parameter
    check(start + 5, _TRANSFER_TOKENS)
      ? transferParams(start, address, encode_address, start + 3, input)->Option.map(res => (
          res,
          start + 6,
        ))
      : None
  }

  // See MANAGER_LAMBDA.call
  // [
  //   { prim: 'DROP' },
  //   { prim: 'NIL', args: [{ prim: 'operation' }] },
  // 0 { prim: 'PUSH', args: [{ prim: 'address' }, { string: <CONTRACT_ADDRESS%entrypoint> }], },
  // 1 { prim: 'CONTRACT', args: [{ prim: <PARAMETER_TYPE> }] },
  // 2 [ { prim: 'IF_NONE', args: [[[{ prim: 'UNIT' }, { prim: 'FAILWITH' }]], []] }, ],
  // 3 { prim: 'PUSH', args: [{ prim: 'mutez' }, { int: <AMOUNT> }] },
  // 4 { prim: 'PUSH', args: [PARAMETER_TYPE, PARAMETER_VALUE] },
  // 5 { prim: 'TRANSFER_TOKENS' },
  //   { prim: 'CONS' },
  // ]
  let call = (input: array<Js.Json.t>, start) => {
    let check = (pos, value) => check(input, pos, value)
    let token_amount = check(start + 1, _CONTRACT(fa12_parameter_type))
      ? Some(fa12_token_amount)
      : check(start + 1, _CONTRACT(fa2_parameter_type))
      ? Some(fa2_token_amount)
      : None
    token_amount->Option.flatMap(token_amount =>
      check(start + 2, seq([_IF_NONE(seq([seq([_UNIT, _FAILWITH])]), seq([]))])) &&
      check(start + 5, _TRANSFER_TOKENS)
        ? recipient_token_amount(
            start,
            address,
            s => s->encode_address->fst,
            start + 3,
            start + 4,
            token_amount,
            input,
          )->Option.map(res => (res, start + 6))
        : None
    )
  }

  let transfers = (input: array<Js.Json.t>, start): option<(array<transferParams>, int)> =>
    switch transferImplicit(input, start) {
    | None =>
      switch transferToContract(input, start) {
      | None => call(input, start)
      | Some(params, next) => Some([params], next)
      }
    | Some(params, next) => Some([params], next)
    }

  let delegate = (input: array<Js.Json.t>, start): option<(option<PublicKeyHash.t>, int)> =>
    switch setDelegate(input, start) {
    | None => removeDelegate(input, start)->Option.map(next => (None, next))
    | Some(x, next) => Some(Some(x), next)
    }

  let parseOperationsList = (json: Js.Json.t) => {
    Js.Json.decodeArray(json)->Option.flatMap(input => {
      let last = Array.length(input) - 1
      let check = (pos, value) => check(input, pos, value)
      check(0, _DROP) && check(1, _NIL(operation))
        ? {
            let rec parse = (acc, instr) => {
              instr == 2 || check(instr, _CONS)
                ? instr == last
                    ? Some(acc)
                    : {
                        let instr = instr == 2 ? 2 : instr + 1
                        open Operation
                        switch transfers(input, instr)->Option.map(((transactions, next)) => (
                          transactions->Array.map(x => x->Transaction),
                          next,
                        )) {
                        | Some(x, next) => parse(Js.Array.concat(acc, x), next)
                        | None =>
                          delegate(input, instr)
                          ->Option.map(x => {
                            let (delegate, next) = x
                            ({Delegation.delegate: delegate}->Delegation, next)
                          })
                          ->Option.mapWithDefault(Some([Unknown]), x => {
                            let (x, next) = x
                            parse(Js.Array.concat(acc, [x]), next)
                          })
                        }
                      }
                : None
            }
            parse([], 2)
          }
        : None
    })
  }
}
