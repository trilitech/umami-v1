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

let defaultAsset = {
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

let thumbnailUriFromFormat = (thumbnailUri, formats) =>
  switch (formats) {
  | None => thumbnailUri
  | Some(formats) =>
    formats->Array.some(({uri}) => uri == thumbnailUri) ? thumbnailUri : None
  };

module Decode = {
  open Json.Decode;

  let dimensionsDecoder: Js.Json.t => dimensions =
    json => {
      value: json |> field("value", string),
      unit: json |> field("unit", string),
    };

  let dataRateDecoder: Js.Json.t => dataRate =
    json => {
      value: json |> field("value", int),
      unit: json |> field("unit", string),
    };

  let formatDecoder = json => {
    uri: json |> optional(field("uri", string)),
    hash: json |> optional(field("hash", string)),
    mimeType: json |> optional(field("mimeType", string)),
    fileSize: json |> optional(field("fileSize", int)),
    fileName: json |> optional(field("fileName", string)),
    duration: json |> optional(field("duration", string)),
    dimensions: json |> optional(field("dimensions", dimensionsDecoder)),
    dataRate: json |> optional(field("dataRate", dataRateDecoder)),
  };

  let attributeDecoder: Js.Json.t => attribute =
    json => {
      name: json |> field("name", string),
      value: json |> field("value", string),
      type_: json |> optional(field("type", string)),
    };

  let assetDecoder = json => {
    description: json |> optional(field("description", string)),
    minter: json |> optional(field("minter", PublicKeyHash.decoder)),
    creators: json |> optional(field("creators", array(string))),
    contributors: json |> optional(field("contributors", array(string))),
    publishers: json |> optional(field("publishers", array(string))),
    date: json |> optional(field("date", string)),
    blocklevel: json |> optional(field("blocklevel", int)),
    type_: json |> optional(field("type", string)),
    tags: json |> optional(field("tags", array(string))),
    genres: json |> optional(field("genres", array(string))),
    language: json |> optional(field("language", string)),
    identifier: json |> optional(field("identifier", string)),
    rights: json |> optional(field("rights", string)),
    rightUri: json |> optional(field("rightUri", string)),
    artifactUri: json |> optional(field("artifactUri", string)),
    displayUri: json |> optional(field("displayUri", string)),
    thumbnailUri: json |> optional(field("thumbnailUri", string)),
    isTransferable:
      (json |> optional(field("isTransferable", bool)))
      ->Option.getWithDefault(true), // default: true
    isBooleanAmount:
      (json |> optional(field("isBooleanAmount", bool)))
      ->Option.getWithDefault(false), // default: false
    shouldPreferSymbol:
      (json |> optional(field("shouldPreferSymbol", bool)))
      ->Option.getWithDefault(false),
    formats: json |> optional(field("formats", array(formatDecoder))),
    attributes:
      json |> optional(field("attributes", array(attributeDecoder))),
  };
};

module Encode = {
  open Json.Encode;

  let dimensionsEncoder = (d: dimensions) => {
    object_([("value", d.value |> string), ("unit", d.unit |> string)]);
  };

  let dataRateEncoder = (d: dataRate) => {
    object_([("value", d.value |> int), ("unit", d.unit |> string)]);
  };

  let formatEncoder = format => {
    object_([
      ("uri", format.uri |> nullable(string)),
      ("hash", format.hash |> nullable(string)),
      ("mimeType", format.mimeType |> nullable(string)),
      ("fileSize", format.fileSize |> nullable(int)),
      ("fileName", format.fileName |> nullable(string)),
      ("duration", format.duration |> nullable(string)),
      ("dimensions", format.dimensions |> nullable(dimensionsEncoder)),
      ("dataRate", format.dataRate |> nullable(dataRateEncoder)),
    ]);
  };

  let attributeEncoder = (a: attribute) => {
    object_([
      ("name", a.name |> string),
      ("value", a.value |> string),
      ("type", a.type_ |> nullable(string)),
    ]);
  };

  let assetEncoder = asset => {
    object_([
      ("description", asset.description |> nullable(string)),
      ("minter", (asset.minter :> option(string)) |> nullable(string)),
      ("creators", asset.creators |> nullable(array(string))),
      ("contributors", asset.contributors |> nullable(array(string))),
      ("publishers", asset.publishers |> nullable(array(string))),
      ("date", asset.date |> nullable(string)),
      ("blocklevel", asset.blocklevel |> nullable(int)),
      ("type", asset.type_ |> nullable(string)),
      ("tags", asset.tags |> nullable(array(string))),
      ("genres", asset.genres |> nullable(array(string))),
      ("language", asset.language |> nullable(string)),
      ("identifier", asset.identifier |> nullable(string)),
      ("rights", asset.rights |> nullable(string)),
      ("rightUri", asset.rightUri |> nullable(string)),
      ("artifactUri", asset.artifactUri |> nullable(string)),
      ("displayUri", asset.displayUri |> nullable(string)),
      ("thumbnailUri", asset.thumbnailUri |> nullable(string)),
      ("isTransferable", asset.isTransferable |> bool),
      ("isBooleanAmount", asset.isBooleanAmount |> bool),
      ("shouldPreferSymbol", asset.shouldPreferSymbol |> bool),
      ("formats", asset.formats |> nullable(array(formatEncoder))),
      ("attributes", asset.attributes |> nullable(array(attributeEncoder))),
    ]);
  };
};
