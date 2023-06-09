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

module Registered: {
  include LocalStorage.StorageType with type t = RegisteredTokens.t

  let getWithFallback: unit => Promise.result<t>
}

@ocaml.doc(" The cache is a representation of the already fetched tokens from the chain ")
module Cache: {
  // Cache in localStorage
  include LocalStorage.StorageType with type t = TokensLibrary.t

  let getWithFallback: unit => Promise.result<t>
}

let mergeAccountNFTs: (
  RegisteredTokens.t,
  TokensLibrary.WithBalance.t,
  PublicKeyHash.t,
) => RegisteredTokens.t

module Legacy: {
  module V1_3: {
    let version: Version.t
    let mk: unit => Promise.result<unit>
  }
  module V1_4: {
    let version: Version.t
    let mk: unit => Promise.result<unit>
  }
  module V1_6: {
    let version: Version.t
    let mk: unit => Promise.result<unit>
  }
  module V1_7: {
    let version: Version.t
    let mk: unit => Promise.result<unit>
  }
}
