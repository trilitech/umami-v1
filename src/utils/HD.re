module Buffer = Js.TypedArray2.Uint8Array;

/*
 [@bs.val] [@bs.scope "Buffer"] external nodeBufferFromBytes: Buffer.t => Node_buffer.t = "from";

 module Node_bufferEx = {
   [@bs.get] external buffer: Node_buffer.t => Js.Typed_array.array_buffer = "buffer";
 };
 */

[@bs.send] external toHex: (Buffer.t, [@bs.as "hex"] _) => string = "toString";

module ED25519 = {
  type t = {
    key: Buffer.t,
    chainCode: Buffer.t,
  };

  [@bs.module "ed25519-hd-key"]
  external derivePath: (string, string) => t = "derivePath";
  //[@bs.module "ed25519-hd-key"]
  //external getMasterKeyFromSeed: Buffer.t => t = "getMasterKeyFromSeed";
};

module BIP39 = {
  [@bs.module "bip39"]
  external seed: string => Buffer.t = "mnemonicToSeedSync";
};

module BS58Check = {
  [@bs.module "bs58check"] external encode: Buffer.t => string = "encode";
  //[@bs.module "bs58check"] external decode: string => Buffer.t = "decode";
};

let b58cencode = (data, prefix) => {
  let buffer =
    Buffer.fromBuffer(
      Js.TypedArray2.ArrayBuffer.make(
        prefix->Js.Array2.length + data->Buffer.length,
      ),
    );
  buffer->Buffer.setArray(prefix);
  buffer->BufferEx.setArrayAt(data, prefix->Js.Array2.length);
  BS58Check.encode(buffer);
};

let toEDSK = seed => {
  b58cencode(
    seed->Buffer.slice(~start=0, ~end_=32),
    [|13, 15, 58, 7|] // edsk
  );
};
/*
 let getMainDerivationPath = index => {
   {j|m/44'/1729'/$(index)'/0'|j};
 };

 let seedToHDPrivateKey = (seed, index) => {
   toEDSK(derivePath(getMainDerivationPath(index), seed));
 };
 */

module Sodium = {
  [@bs.module "libsodium-wrappers-sumo"] external crypto_sign_seed_keypair: (Buffer.t, [@bs.as "uint8array"] _) => Buffer.t = "crypto_sign_seed_keypair";
};

module PBKDF2 = {
  [@bs.module "pbkdf2"]
  external pbkdf2Sync:
    (string, Buffer.t, [@bs.as 32768] _, [@bs.as 32] _, [@bs.as "sha512"] _) =>
    Buffer.t =
    "pbkdf2sync";
};

let edesk = (path, recoveryPhrase, ~password) => {
  let secretKey =
    ED25519.derivePath(path, recoveryPhrase->BIP39.seed->toHex).key;
  let privateKey = secretKey->Sodium.crypto_sign_seed_keypair;
  Js.log(privateKey);
};
