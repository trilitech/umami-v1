module Unit: {
  type t = private ReBigNumber.t

  let isNat: t => bool

  type illformed =
    | NaN
    | Float(option<int>)
    | Negative

  type Errors.t +=
    | IllformedTokenUnit(ReBigNumber.t, illformed)

  let fromBigNumber: ReBigNumber.t => Promise.result<t>
  let toBigNumber: t => ReBigNumber.t

  let fromFloatBigNumber: (ReBigNumber.t, int) => Promise.result<t>

  let toNatString: (~decimals: int=?, t) => string
  let fromNatString: string => Promise.result<t>

  let toStringDecimals: (t, int) => string
  let fromStringDecimals: (string, int) => Promise.result<t>

  let formatString: (string, int) => Promise.result<string>

  let zero: t
  let one: t
  let forceFromString: string => option<t>

  let add: (t, t) => t

  module Infix: {
    let \"+": (t, t) => t
  }
}

type address = PublicKeyHash.t

type kind =
  | FA1_2
  | FA2(int)

type t = {
  kind: kind,
  address: address,
  alias: string,
  symbol: string,
  chain: Network.chainId,
  decimals: int,
  asset: Metadata.asset,
}

let toFlatJson: t => option<Js.Json.t>

let kindId: kind => int
// let idKind: int => kind;
let id: t => int

let isNFT: t => bool

let isFa2: t => bool

let decoder: Js.Json.t => Unit.t

type token = (PublicKeyHash.t, option<int>)

module Comparator: Belt.Id.Comparable with type t = (PublicKeyHash.t, token)
module Map: Map.S with module Key := Comparator
