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

module Buffer = {
  type t = Node_buffer.t;

  [@bs.val] [@bs.scope "Buffer"] external alloc: int => t = "alloc";
  [@bs.val] [@bs.scope "Buffer"]
  external allocWithString: (int, string) => t = "alloc";

  [@bs.val] [@bs.scope "Buffer"]
  external fromBytes: Js.TypedArray2.Uint8Array.t => t = "from";
  [@bs.val] [@bs.scope "Buffer"]
  external fromString: (string, Node_buffer.encoding) => t = "from";

  let toString = (t, encoding) =>
    t->Node_buffer.toStringWithEncoding(encoding);
};

module Crypto = {
  type uint8Array = Js.TypedArray2.Uint8Array.t;

  [@bs.val] [@bs.scope "crypto"]
  external getRandomValues: uint8Array => uint8Array = "getRandomValues";

  let allocAndFillWithRandomValues = size =>
    getRandomValues(
      Js.TypedArray2.(Uint8Array.fromBuffer(ArrayBuffer.make(size))),
    );

  type cryptoKey;

  module Subtle = {
    [@bs.val] [@bs.scope "crypto.subtle"]
    external importKey:
      (
        [@bs.as "raw"] _,
        Buffer.t,
        [@bs.as "PBKDF2"] _,
        bool,
        array(string)
      ) =>
      Js.Promise.t(cryptoKey) =
      "importKey";

    type derivationAlgorithm = {
      name: string,
      salt: Buffer.t,
      iterations: int,
      hash: string,
    };

    type derivedKeyAlgorithm = {
      name: string,
      length: int,
    };

    [@bs.val] [@bs.scope "crypto.subtle"]
    external deriveKey:
      (
        derivationAlgorithm,
        cryptoKey,
        derivedKeyAlgorithm,
        bool,
        array(string)
      ) =>
      Js.Promise.t(cryptoKey) =
      "deriveKey";

    type encryptionAlgorithm = {
      name: string,
      iv: uint8Array,
    };

    [@bs.val] [@bs.scope "crypto.subtle"]
    external encrypt:
      (encryptionAlgorithm, cryptoKey, Buffer.t) => Js.Promise.t(uint8Array) =
      "encrypt";

    type decryptionAlgorithm = {
      name: string,
      iv: Buffer.t,
    };

    [@bs.val] [@bs.scope "crypto.subtle"]
    external decrypt:
      (decryptionAlgorithm, cryptoKey, Buffer.t) => Js.Promise.t(uint8Array) =
      "decrypt";
  };
};

module Cipher = {
  type Errors.t +=
    | KeyFromPasswordError(string)
    | DeriveKeyError(string)
    | DecryptError(string)
    | EncryptError(string);

  let () =
    Errors.registerHandler(
      "SecureStorage",
      fun
      | KeyFromPasswordError(s) => s->Some
      | DeriveKeyError(s) => s->Some
      | DecryptError(s) => s->Some
      | EncryptError(s) => s->Some
      | _ => None,
    );

  type encryptedData = {
    salt: string,
    iv: string,
    data: string,
  };

  let decoder = json =>
    Json.Decode.{
      salt: json |> field("salt", string),
      iv: json |> field("iv", string),
      data: json |> field("data", string),
    };

  let encoder = encryptedData =>
    Json.Encode.(
      object_([
        ("salt", string(encryptedData.salt)),
        ("iv", string(encryptedData.iv)),
        ("data", string(encryptedData.data)),
      ])
    );

  let keyFromPassword = password => {
    let buffer = Buffer.allocWithString(32, password);
    Crypto.Subtle.importKey(buffer, false, [|"deriveBits", "deriveKey"|])
    ->RawJsError.fromPromiseParsed(e => KeyFromPasswordError(e.message));
  };

  let deriveKey = (key, salt) =>
    Crypto.Subtle.deriveKey(
      {name: "PBKDF2", salt, iterations: 10000, hash: "SHA-256"},
      key,
      {name: "AES-GCM", length: 256},
      false,
      [|"encrypt", "decrypt"|],
    )
    ->RawJsError.fromPromiseParsed(e => DeriveKeyError(e.message));

  let encrypt = (data, password) => {
    let%FRes key = keyFromPassword(password);
    let salt = Buffer.fromBytes(Crypto.allocAndFillWithRandomValues(32));

    let%FRes derivedKey = deriveKey(key, salt);
    let iv = Crypto.allocAndFillWithRandomValues(16);

    let%FResMap encryptedData =
      Crypto.Subtle.encrypt(
        {name: "AES-GCM", iv},
        derivedKey,
        Buffer.fromString(data, `utf8),
      )
      ->RawJsError.fromPromiseParsed(e => EncryptError(e.message));

    {
      salt: salt->Buffer.toString(`hex),
      iv: Buffer.fromBytes(iv)->Buffer.toString(`hex),
      data: Buffer.fromBytes(encryptedData)->Buffer.toString(`hex),
    };
  };

  let encrypt2 = (password, data) => encrypt(data, password);

  let decrypt = (encryptedData, password) => {
    let%FRes key = keyFromPassword(password);

    let%FRes derivedKey =
      deriveKey(key, Buffer.fromString(encryptedData.salt, `hex));

    let%FResMap data =
      Crypto.Subtle.decrypt(
        {name: "AES-GCM", iv: Buffer.fromString(encryptedData.iv, `hex)},
        derivedKey,
        Buffer.fromString(encryptedData.data, `hex),
      )
      ->RawJsError.fromPromiseParsed(e => DecryptError(e.message));

    Buffer.fromBytes(data)->Buffer.toString(`utf8);
  };

  let decrypt2 = (password, encryptedData) =>
    decrypt(encryptedData, password);
};

type json = Js.Json.t;

let fetchEncryptedData = key =>
  LocalStorage.getItem(key)
  ->Js.Nullable.toOption
  ->Option.flatMap(Json.parse)
  ->Option.map(Cipher.decoder);

let storeEncryptedData = (data, ~key) =>
  data->Cipher.encoder->Json.stringify |> LocalStorage.setItem(key);

let fetch = (key, ~password) =>
  switch (fetchEncryptedData(key)) {
  | Some(encryptedData) =>
    encryptedData->Cipher.decrypt(password)->Future.mapOk(data => Some(data))
  | None => Future.value(Ok(None))
  };

let store = (data, ~key, ~password) =>
  data
  ->Cipher.encrypt(password)
  ->Future.mapOk(encryptedData => encryptedData->storeEncryptedData(~key));

let validatePassword = password => {
  let%FRes data = "lock"->fetch(~password);
  let%FRes () =
    data == Some("lock") || data == None
      ? FutureEx.ok() : FutureEx.err(Errors.WrongPassword);
  let%FResMap () = store("lock", ~key="lock", ~password);
  ();
};
