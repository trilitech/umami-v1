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

/* ALIAS */

/* Get list */

let useLoad = requestState => {
  let get = (~config, ()) =>
    HDWalletAPI.Aliases.get(~config)->Promise.map(x =>
      switch x {
      | Ok(response) =>
        response->Array.map(a => (a.address, a))->Array.reverse->PublicKeyHash.Map.fromArray->Ok
      | Error(System.NoSuchFileError(_)) => PublicKeyHash.Map.empty->Ok
      | Error(_) as e => e
      }
    )

  ApiRequest.useLoader(~get, ~kind=Logs.Aliases, ~requestState, ())
}

/* Create */

let useCreate = ApiRequest.useSetter(
  ~logOk=_ => I18n.contact_added,
  ~set=(~config, (alias, address)) => HDWalletAPI.Aliases.add(~config, ~alias, ~address),
  ~kind=Logs.Aliases,
)

/* Update */

let useUpdate = ApiRequest.useSetter(
  ~logOk=_ => I18n.contact_updated,
  ~set=(~config, renaming: HDWalletAPI.Aliases.renameParams) =>
    HDWalletAPI.Aliases.rename(~config, renaming),
  ~kind=Logs.Aliases,
)

/* Delete */

let useDelete = {
  let set = (~config, alias) => HDWalletAPI.Aliases.delete(~config, ~alias)
  ApiRequest.useSetter(~logOk=_ => I18n.contact_deleted, ~set, ~kind=Logs.Aliases)
}
