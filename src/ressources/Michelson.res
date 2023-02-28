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

  let dataDecoder = params => field("data", array(params))

  let pairDecoder = (d1, d2) => field("args", tuple2(d1, d2))

  let intDecoder = field("int", string)

  let bytesDecoder = field("bytes", string) |> map(s => s->Bytes.unsafe_of_string)

  let stringDecoder = field("string", string)

  let addressDecoder = either(
    bytesDecoder |> map(b => Packed(b)),
    stringDecoder |> map(s => Pkh(s->PublicKeyHash.build->JsonEx.getExn)),
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
  let int = (int: string): Js.Json.t => {"int": int}->Obj.magic
  let string = (string: string): Js.Json.t => {"string": string}->Obj.magic
  let bytes = (bytes: string): Js.Json.t => {"bytes": bytes}->Obj.magic
  let prim = (prim: string, args: array<Js.Json.t>): Js.Json.t =>
    [] == args ? {"prim": prim}->Obj.magic : {"prim": prim, "args": args}->Obj.magic
  let array = (array: array<Js.Json.t>): Js.Json.t => array->Obj.magic
}

module LAMBDA_PARSER = {
  open MichelsonV1Expression

  let check = (input, pos, value) => input[pos]->Option.mapWithDefault(false, x => x == value)

  // For a given <FIELD> value:
  // read {prim: "PUSH", args: [{prim: "<FIELD>"}, {bytes: <RECIPIENT>}]} and
  // return <RECIPIENT>
  let recipient = (field, encode, json) => {
    Js.Json.decodeObject(json)
    ->Option.flatMap(x =>
      Js_dict.get(x, "prim") == Some(Obj.magic("PUSH")) ? Js_dict.get(x, "args") : None
    )
    ->Option.flatMap(Js.Json.decodeArray)
    ->Option.flatMap(x => x[0] == Some(prim(field, [])) ? x[1] : None)
    ->Option.flatMap(Js.Json.decodeObject)
    ->Option.flatMap(x => Js.Dict.get(x, "bytes")) // is "string" in LAMBDA_MANAGER
    ->Option.flatMap(Js.Json.decodeString)
    ->Option.map(encode)
    ->Option.map(PublicKeyHash.build)
    ->Option.map(Result.getExn)
  }

  // Extract <AMOUNT> from {prim: "PUSH", args: [{prim: "mutez"}, {int: <AMOUNT>}]}
  let amount = json =>
    Js.Json.decodeObject(json)
    ->Option.flatMap(x =>
      Js_dict.get(x, "prim") == Some(Obj.magic("PUSH")) ? Js_dict.get(x, "args") : None
    )
    ->Option.flatMap(Js.Json.decodeArray)
    ->Option.flatMap(x => x[0] == Some(prim("mutez", [])) ? x[1] : None)
    ->Option.flatMap(Js.Json.decodeObject)
    ->Option.flatMap(x => Js.Dict.get(x, "int"))
    ->Option.flatMap(Js.Json.decodeString)
    ->Option.map(Tez.fromMutezString)

  type recipient_amount = {
    recipient: PublicKeyHash.t,
    amount: Tez.t,
  }

  let recipient_amount = (r, rk, encode, a, input): option<recipient_amount> => {
    input[r]
    ->Option.flatMap(recipient(rk, encode))
    ->Option.flatMap(recipient => {
      input[a]
      ->Option.flatMap(amount)
      ->Option.map(amount => {
        {recipient: recipient, amount: amount}
      })
    })
  }

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
    check(start + 1, prim("SOME", [])) && check(start + 2, prim("SET_DELEGATE", []))
      ? input[start]
        ->Option.flatMap(recipient("key_hash", ReTaquitoUtils.encodeKeyHash))
        ->Option.map(res => (res, start + 3))
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
    check(start, prim("NONE", [prim("key_hash", [])])) && check(start + 1, prim("SET_DELEGATE", []))
      ? Some(start + 2)
      : None
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
    check(start + 1, prim("IMPLICIT_ACCOUNT", [])) &&
    check(start + 3, prim("UNIT", [])) &&
    check(start + 4, prim("TRANSFER_TOKENS", []))
      ? recipient_amount(
          start,
          "key_hash",
          ReTaquitoUtils.encodeKeyHash,
          start + 2,
          input,
        )->Option.map(res => (res, start + 5))
      : None
  }

  // See MANAGER_LAMBDA.transferToContract
  // [
  //   { prim: 'DROP' },
  //   { prim: 'NIL', args: [{ prim: 'operation' }] },
  // 0 { prim: 'PUSH', args: [{ prim: 'address' }, { string: key }], },
  // 1 { prim: 'CONTRACT', args: [{ prim: 'unit' }] },
  // 2 [ { prim: 'IF_NONE', args: [[[{ prim: 'UNIT' }, { prim: 'FAILWITH' }]], []], }, ],
  // 3 { prim: 'PUSH', args: [{ prim: 'mutez' }, { int: `${amount}` }], },
  // 4 { prim: 'UNIT' },
  // 5 { prim: 'TRANSFER_TOKENS' },
  //   { prim: 'CONS' },
  // ]
  let transferToContract = (input: array<Js.Json.t>, start) => {
    let check = (pos, value) => check(input, pos, value)
    check(start + 1, prim("CONTRACT", [prim("unit", [])])) &&
    check(
      start + 2,
      array([
        prim("IF_NONE", [array([array([prim("UNIT", []), prim("FAILWITH", [])])]), array([])]),
      ]),
    ) &&
    check(start + 4, prim("UNIT", [])) &&
    check(start + 5, prim("TRANSFER_TOKENS", []))
      ? recipient_amount(
          start,
          "address",
          ReTaquitoUtils.encodePubKey,
          start + 3,
          input,
        )->Option.map(res => (res, start + 6))
      : None
  }

  let transfer = (input: array<Js.Json.t>, start): option<(recipient_amount, int)> =>
    switch transferImplicit(input, start) {
    | None => transferToContract(input, start)
    | x => x
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
      check(0, prim("DROP", [])) && check(1, prim("NIL", [prim("operation", [])]))
        ? {
            let rec parse = (acc, instr) => {
              instr == 2 || check(instr, prim("CONS", []))
                ? instr == last
                    ? Some(acc)
                    : {
                        let instr = instr == 2 ? 2 : instr + 1
                        open Operation
                        switch transfer(input, instr)->Option.map(res => {
                          let ({amount, recipient}, next) = res
                          (
                            Transaction.Tez({
                              amount: amount,
                              destination: recipient,
                              parameters: None,
                              entrypoint: None,
                            })->Transaction,
                            next,
                          )
                        }) {
                        | Some(x, next) => parse(Js.Array.concat(acc, [x]), next)
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
