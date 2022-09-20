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
