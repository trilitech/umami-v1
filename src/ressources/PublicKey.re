module Scheme = {
  type t =
    | ED25519
    | SECP256K1
    | P256;

  let toString =
    fun
    | ED25519 => "ED25519"
    | SECP256K1 => "SECP256K1"
    | P256 => "P256";
};

type implicit =
  | TZ1
  | TZ2
  | TZ3;

type kind =
  | Implicit(implicit)
  | KT1;

let implicitFromScheme =
  fun
  | Scheme.ED25519 => TZ1
  | SECP256K1 => TZ2
  | P256 => TZ3;

let kindToString =
  fun
  | Implicit(TZ1) => "tz1"
  | Implicit(TZ2) => "tz2"
  | Implicit(TZ3) => "tz3"
  | KT1 => "kt1";
