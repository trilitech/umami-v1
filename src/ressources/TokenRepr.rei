module Unit: {
  type t = pri ReBigNumber.t;

  let isNat: t => bool;

  type illformed =
    | NaN
    | Float
    | Negative;

  let fromBigNumber: ReBigNumber.t => result(t, illformed);
  let toBigNumber: t => ReBigNumber.t;

  let toNatString: (~decimals: int=?, t) => string;
  let fromNatString: string => result(t, illformed);

  let toStringDecimals: (t, int) => string;
  let fromStringDecimals: (string, int) => result(t, illformed);

  let formatString: (string, int) => Belt.Result.t(string, illformed);

  let zero: t;
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
