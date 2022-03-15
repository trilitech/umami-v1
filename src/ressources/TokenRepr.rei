module Unit: {
  type t = pri ReBigNumber.t;

  let isNat: t => bool;

  type illformed =
    | NaN
    | Float(option(int))
    | Negative;

  type Errors.t +=
    | IllformedTokenUnit(ReBigNumber.t, illformed);

  let fromBigNumber: ReBigNumber.t => Promise.result(t);
  let toBigNumber: t => ReBigNumber.t;

  let fromFloatBigNumber: (ReBigNumber.t, int) => Promise.result(t);

  let toNatString: (~decimals: int=?, t) => string;
  let fromNatString: string => Promise.result(t);

  let toStringDecimals: (t, int) => string;
  let fromStringDecimals: (string, int) => Promise.result(t);

  let formatString: (string, int) => Promise.result(string);

  let zero: t;
  let one: t;
  let forceFromString: string => option(t);

  let add: (t, t) => t;

  module Infix: {let (+): (t, t) => t;};
};

type address = PublicKeyHash.t;

type kind =
  | FA1_2
  | FA2(int);

module Metadata: {
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

let thumbnailUriFromFormat:
  (option(string), option(array(Metadata.format))) => option(string);

let defaultAsset: Metadata.asset;

type t = {
  kind,
  address,
  alias: string,
  symbol: string,
  chain: string,
  decimals: int,
  asset: Metadata.asset,
};

let toFlatJson: t => option(Js.Json.t);

let kindId: kind => int;
let id: t => int;

let isNFT: t => bool;

let isFa2: t => bool;
