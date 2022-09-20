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

open TokenRepr

let ipfsService = "https://ipfs.io/ipfs/"

module ImageProxy = {
  let buildUrl = url => {
    let encodedURL = Js.Global.encodeURI(url)

    ServerAPI.URL.build_url(
      "https://dev-api.umamiwallet.com/mainnet-graphic-proxy",
      list{("url", encodedURL)},
    )
  }
}

type ressource =
  | IPFS(string)
  | HTTP(string)

let toRessource = uri =>
  uri->Js.String2.startsWith("ipfs://") ? uri->Js.String2.substringToEnd(~from=7)->IPFS : HTTP(uri)

let ressourceToURL = r =>
  switch r {
  | IPFS(ipfsHash) => ipfsService ++ ipfsHash
  | HTTP(url) => ImageProxy.buildUrl(url)
  }

let getDisplayURL = token =>
  token.asset.displayUri->Option.map(toRessource)->Option.map(d => ressourceToURL(d))

let getThumbnailURL = token =>
  switch token.asset.thumbnailUri->Option.map(toRessource) {
  | Some(thl) => ressourceToURL(thl)->Some
  | None => getDisplayURL(token)
  }
