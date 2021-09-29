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

type Errors.t +=
  | DecodeError(string);

// Propagates Errors.t during decoding, should be caught by the decode function
exception InternalError(Errors.t);

let decode:
  (Js.Json.t, Json.Decode.decoder('a)) => result('a, TezosClient.Errors.t);

module MichelsonDecode: {
  type address =
    | Packed(bytes)
    | Pkh(PublicKeyHash.t);

  let dataDecoder:
    Json.Decode.decoder('a) => Json.Decode.decoder(array('a));

  let pairDecoder:
    (Json.Decode.decoder('a), Json.Decode.decoder('b)) =>
    Json.Decode.decoder(('a, 'b));

  let intDecoder: Json.Decode.decoder(string);

  let bytesDecoder: Json.Decode.decoder(bytes);

  let stringDecoder: Json.Decode.decoder(string);

  let addressDecoder: Json.Decode.decoder(address);

  let fa2BalanceOfDecoder: Js.Json.t => array(((address, string), string));
};
