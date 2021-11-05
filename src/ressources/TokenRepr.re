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

module Unit = {
  type t = ReBigNumber.t;

  type illformed =
    | NaN
    | Float
    | Negative;

  open ReBigNumber;
  let toBigNumber = x => x;
  let fromBigNumber = x =>
    if (x->isNaN) {
      Error(NaN);
    } else if (!x->isInteger) {
      Error(Float);
    } else if (x->isNegative) {
      Error(Negative);
    } else {
      x->Ok;
    };

  let isNat = v => v->isInteger && !v->isNegative && !v->isNaN;

  let toNatString = toFixed;
  let toStringDecimals = (x, decimals) => {
    let shift = fromInt(10)->powInt(decimals);
    let x = x->div(shift);
    toFixed(~decimals, x);
  };
  let fromNatString = s => s->fromString->fromBigNumber;
  let fromStringDecimals = (s, decimals) => {
    let shift = fromInt(10)->powInt(decimals);
    let x = s->fromString->times(shift);
    x->fromBigNumber;
  };

  let formatString = (s, decimals) => {
    let x = fromStringDecimals(s, decimals);
    x->Result.map(x => toStringDecimals(x, decimals));
  };

  let forceFromString = s => {
    let v = s->fromString;
    v->isNaN ? None : v->isInteger ? v->integerValue->Some : None;
  };

  let one = fromString("1");
  let zero = fromString("0");

  let add = plus;

  module Infix = {
    let (+) = plus;
  };
};

type address = PublicKeyHash.t;

type kind =
  | FA1_2
  | FA2(int);

module Metadata = {
  type dimensions = {
    value: string,
    unit: string,
  };

  type dataRate = {
    value: int,
    unit: string,
  };

  type format = {
    uri: option(string),
    hash: option(string),
    mimeType: option(string),
    fileSize: option(int),
    fileName: option(string),
    duration: option(string),
    dimensions: option(dimensions),
    dataRate: option(dataRate),
  };

  type attribute = {
    name: string,
    value: string,
    type_: option(string),
  };

  type asset = {
    description: option(string),
    minter: option(PublicKeyHash.t),
    creators: option(array(string)),
    contributors: option(array(string)),
    publishers: option(array(string)),
    date: option(string),
    blocklevel: option(int),
    type_: option(string),
    tags: option(array(string)),
    genres: option(array(string)),
    language: option(string),
    identifier: option(string),
    rights: option(string),
    rightUri: option(string),
    artifactUri: option(string),
    displayUri: option(string),
    thumbnailUri: option(string),
    isTransferable: bool, // default: true
    isBooleanAmount: bool, // default: false
    shouldPreferSymbol: bool, //default: false
    formats: option(array(format)),
    attributes: option(array(attribute)),
  };
};

let defaultAsset =
  Metadata.{
    description: None,
    minter: None,
    creators: None,
    contributors: None,
    publishers: None,
    date: None,
    blocklevel: None,
    type_: None,
    tags: None,
    genres: None,
    language: None,
    identifier: None,
    rights: None,
    rightUri: None,
    artifactUri: None,
    displayUri: None,
    thumbnailUri: None,
    isTransferable: true, // default: true
    isBooleanAmount: false, // default: false
    shouldPreferSymbol: false, //default: false
    formats: None,
    attributes: None,
  };

type t = {
  kind,
  address,
  alias: string,
  symbol: string,
  chain: string,
  decimals: int,
  asset: Metadata.asset,
};

let id = token =>
  switch (token.kind) {
  | FA1_2 => 0
  | FA2(n) => n
  };

let isNFT = t =>
  t.asset.artifactUri != None
  || t.asset.displayUri != None
  || t.asset.isBooleanAmount == true;
