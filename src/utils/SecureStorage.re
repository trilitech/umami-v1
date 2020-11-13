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
  type encryptedData = {
    salt: string,
    iv: string,
    data: string,
  };

  let keyFromPassword = password => {
    let buffer = Buffer.allocWithString(32, password);
    Crypto.Subtle.importKey(buffer, false, [|"deriveBits", "deriveKey"|])
    ->FutureJs.fromPromise(Js.String.make);
  };

  let deriveKey = (key, salt) =>
    Crypto.Subtle.deriveKey(
      {name: "PBKDF2", salt, iterations: 10000, hash: "SHA-256"},
      key,
      {name: "AES-GCM", length: 256},
      false,
      [|"encrypt", "decrypt"|],
    )
    ->FutureJs.fromPromise(Js.String.make);

  let encrypt = (data, password) => {
    keyFromPassword(password)
    ->Future.flatMapOk(key => {
        let salt = Buffer.fromBytes(Crypto.allocAndFillWithRandomValues(32));
        deriveKey(key, salt)->Future.mapOk(derivedKey => (salt, derivedKey));
      })
    ->Future.flatMapOk(((salt, derivedKey)) => {
        let iv = Crypto.allocAndFillWithRandomValues(16);
        Crypto.Subtle.encrypt(
          {name: "AES-GCM", iv},
          derivedKey,
          Buffer.fromString(data, `utf8),
        )
        ->FutureJs.fromPromise(Js.String.make)
        ->Future.mapOk(encryptedData =>
            {
              salt: salt->Buffer.toString(`hex),
              iv: Buffer.fromBytes(iv)->Buffer.toString(`hex),
              data: Buffer.fromBytes(encryptedData)->Buffer.toString(`hex),
            }
          );
      });
  };

  let decrypt = (encryptedData, password) => {
    keyFromPassword(password)
    ->Future.flatMapOk(key => {
        deriveKey(key, Buffer.fromString(encryptedData.salt, `hex))
      })
    ->Future.flatMapOk(derivedKey =>
        Crypto.Subtle.decrypt(
          {name: "AES-GCM", iv: Buffer.fromString(encryptedData.iv, `hex)},
          derivedKey,
          Buffer.fromString(encryptedData.data, `hex),
        )
        ->FutureJs.fromPromise(Js.String.make)
      )
    ->Future.mapOk(data => Buffer.fromBytes(data)->Buffer.toString(`utf8));
  };
};

type json = Js.Json.t;

let getEncryptedData = key =>
  LocalStorage.getItem(key)
  ->Js.Nullable.toOption
  ->Belt.Option.flatMap(Json.parse)
  ->Belt.Option.map(json =>
      Json.Decode.{
        Cipher.salt: json |> field("salt", string),
        iv: json |> field("iv", string),
        data: json |> field("data", string),
      }
    );

let setEncryptedData = (key, data) => {
  Json.Encode.(
    object_([
      ("salt", string(data.Cipher.salt)),
      ("iv", string(data.iv)),
      ("data", string(data.data)),
    ])
  )
  ->Json.stringify
  |> LocalStorage.setItem(key);
};
