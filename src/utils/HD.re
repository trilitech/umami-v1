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

open Let;

module Buffer = Js.TypedArray2.Uint8Array;

[@bs.send] external toHex: (Buffer.t, [@bs.as "hex"] _) => string = "toString";
[@bs.send] external toString: Buffer.t => string = "toString";

module ED25519 = {
  type t = {
    key: Buffer.t,
    chainCode: Buffer.t,
  };

  [@bs.module "ed25519-hd-key"]
  external derivePath: (string, string) => t = "derivePath";

  [@bs.module "ed25519-hd-key"]
  external publicKey: string => t = "getPublicKey";
};

module BIP39 = {
  [@bs.module "bip39"]
  external seed: string => Buffer.t = "mnemonicToSeedSync";
};

module BS58Check = {
  [@bs.module "bs58check"] external encode: Buffer.t => string = "encode";
  [@bs.module "bs58check"] external decode: string => Buffer.t = "decode";
};

let b58cencode = (data, prefix) => {
  let buffer =
    Buffer.fromLength(prefix->Js.Array2.length + data->Buffer.length);
  buffer->Buffer.setArray(prefix);
  buffer->BufferEx.setArrayAt(data, prefix->Js.Array2.length);
  BS58Check.encode(buffer);
};

type Errors.t +=
  | SodiumInitError(string)
  | DerivationPathError;

let () =
  Errors.registerHandler(
    "Sodium",
    fun
    | SodiumInitError(s) => s->Some
    | DerivationPathError => I18n.form_input_error#derivation_path_error->Some
    | _ => None,
  );

module Sodium = {
  [@bs.module "libsodium-wrappers-sumo"]
  external ready: Js.Promise.t(unit) = "ready";
  let ready =
    ready->RawJsError.fromPromiseParsed(e => e.message->SodiumInitError);

  [@bs.module "libsodium-wrappers-sumo"]
  external randombytes_buf: int => Buffer.t = "randombytes_buf";

  [@bs.module "libsodium-wrappers-sumo"]
  external crypto_secretbox_easy: (Buffer.t, Buffer.t, Buffer.t) => Buffer.t =
    "crypto_secretbox_easy";

  [@bs.module "libsodium-wrappers-sumo"]
  external crypto_sign_ed25519_sk_to_pk: Buffer.t => Buffer.t =
    "crypto_sign_ed25519_sk_to_pk";
};

[@bs.module "crypto"]
external pbkdf2Sync:
  (string, Buffer.t, [@bs.as 32768] _, [@bs.as 32] _, [@bs.as "sha512"] _) =>
  Buffer.t =
  "pbkdf2Sync";

/** Emulates tezos-client `import from mnemonic` seed generation */
[@bs.module "crypto"]
external pbkdf2MnemonicLegacy:
  (string, string, [@bs.as 2048] _, [@bs.as 64] _, [@bs.as "sha512"] _) =>
  Buffer.t =
  "pbkdf2Sync";

let mergebuf = (b1: Buffer.t, b2: Buffer.t) => {
  let r = Buffer.fromLength(b1->Buffer.length + b2->Buffer.length);
  r->BufferEx.setArray(b1);
  r->BufferEx.setArrayAt(b2, b1->Buffer.length);
  r;
};

let seed = recoveryPhrase => recoveryPhrase->BIP39.seed->toHex;

let edesk = (path, seed, ~password) => {
  let%FlatRes () = Sodium.ready;

  let%ResMap secretKey =
    switch (ED25519.derivePath(path->DerivationPath.toString, seed).key) {
    | key => Ok(key)
    | exception _ => Error(DerivationPathError)
    };

  let salt = Sodium.randombytes_buf(8);
  let encryptionKey = pbkdf2Sync(password, salt);
  let encryptedSecretkey =
    Sodium.crypto_secretbox_easy(
      secretKey,
      Buffer.fromLength(24),
      encryptionKey,
    );
  b58cencode(mergebuf(salt, encryptedSecretkey), [|7, 90, 60, 179, 41|]); // edesk
};

/** Generates an encrypted secret key from a mnemonic. */
let edeskLegacy = (~passphrase="", recoveryPhrase, ~password) => {
  let%FResMap () = Sodium.ready;

  let secretKey =
    pbkdf2MnemonicLegacy(recoveryPhrase, "mnemonic" ++ passphrase)
    ->Buffer.slice(~start=0, ~end_=32);
  let salt = Sodium.randombytes_buf(8);
  let encryptionKey = pbkdf2Sync(password, salt);
  let encryptedSecretkey =
    Sodium.crypto_secretbox_easy(
      secretKey,
      Buffer.fromLength(24),
      encryptionKey,
    );
  b58cencode(mergebuf(salt, encryptedSecretkey), [|7, 90, 60, 179, 41|]); // edesk;
};
