/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

type Errors.t +=
  | CryptoError(string)

type t = {
  version: string,
  salt: string,
  ciphertext: string,
  kdf: string,
}

let decoder = json => {
  open Json.Decode
  {
    version: json |> field("version", string),
    salt: json |> field("salt", string),
    ciphertext: json |> field("ciphertext", string),
    kdf: json |> field("kdf", string),
  }
}

let parse = json => Promise.value(json->JsonEx.decode(decoder))

module CryptoUtils = {
  @module("conseiljs-softsigner") @scope("CryptoUtils")
  external decryptMessage: (HD.Buffer.t, string, HD.Buffer.t) => Js.Promise.t<HD.Buffer.t> =
    "decryptMessage"
}

module Account = {
  type t = {
    publicKeyHash: string,
    publicKey: string,
    secretKey: string, // privateKey || secretKey
  }

  let decoder = json => {
    open Json.Decode
    {
      publicKeyHash: json |> field("publicKeyHash", string),
      publicKey: json |> field("publicKey", string),
      secretKey: json |> field("secretKey", string),
    }
  }

  let toEDESK = (edsk: string, ~password: string) =>
    edsk
    ->ReTaquitoUtils.b58cdecode(ReTaquitoUtils.prefix.edsk)
    ->HD.Buffer.slice(~start=0, ~end_=32)
    ->HD.toEDESK(~password)

  let decode = (t, ~passphrase: string) => {
    let ciphertext = HD.BS58Check.decode(t.ciphertext)
    let salt = HD.BS58Check.decode(t.salt)
    HD.Sodium.ready
    ->Promise.flatMapOk(_ =>
      CryptoUtils.decryptMessage(ciphertext, passphrase, salt)->Promise.fromJs(e =>
        (e->RawJsError.fromPromiseError).message->CryptoError
      )
    )
    ->Promise.mapOk(HD.toString)
    ->Promise.flatMapOk(data => Promise.value(JsonEx.parse(data)))
    ->Promise.flatMapOk(json => Promise.value(json->JsonEx.decode(Json.Decode.array(decoder))))
    ->Promise.flatMapOk(accounts =>
      accounts
      ->Array.map(account =>
        account.secretKey
        ->toEDESK(~password=passphrase)
        ->Promise.mapOk(edesk => {...account, secretKey: edesk})
      )
      ->Promise.allArray
      ->Promise.mapOk(results =>
        results->Array.keepMap(result =>
          switch result {
          | Ok(account) => Some(account)
          | Error(_) => None
          }
        )
      )
    )
  }
}
