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

type kind =
  | Contact
  | Account(Account.kind)
  | Multisig

type t = {
  address: PublicKeyHash.t,
  name: string,
  /* In some specific cases, we need to create an `Alias.t` out of a pkh, which is
   neither a contact nor an account */
  kind: option<kind>,
}

let fromAccount = ({Account.address: address, name, kind}): t => {
  address: address,
  name: name,
  kind: Some(Account(kind)),
}

let toAccountExn = ({address, name, kind}: t): Account.t =>
  switch kind {
  | Some(Account(kind)) => {address: address, name: name, kind: kind}
  | Some(Contact) | Some(Multisig) | None => failwith("toAccountExn")
  }

let toAccount = (x: t): Result.t<Account.t, exn> =>
  try {
    x->toAccountExn->Result.Ok
  } catch {
  | e => e->Result.Error
  }

let fromMultisig = ({Multisig.address: address, alias}): t => {
  address: address,
  name: alias,
  kind: Some(Multisig),
}

let make = (~kind=?, ~name, address) => {address: address, name: name, kind: kind}

let compareName = (a, b) => Pervasives.compare(a.name, b.name)

let compareAddress = (a, b) => Pervasives.compare(a.address, b.address)

let compareKind = (a, b) =>
  switch (a.kind, b.kind) {
  | (Some(Account(a)), Some(Account(b))) =>
    a == b
      ? 0
      : switch (a, b) {
        | (Encrypted | Unencrypted, _) => -1
        | (_, Encrypted | Unencrypted) => 1
        | (Ledger, _) => -1
        | (_, Ledger) => 1
        | (_, _) => Pervasives.compare(a, b)
        }
  | (Some(Contact), Some(Contact)) | (Some(Multisig), Some(Multisig)) | (None, None) => 0
  | (Some(Account(_)), Some(Multisig | Contact) | None) => -1
  | (Some(Multisig | Contact) | None, Some(Account(_))) => 1
  | (Some(Multisig), Some(Contact) | None) => -1
  | (Some(Contact) | None, Some(Multisig)) => 1
  | (Some(Contact), None) => -1
  | (None, Some(Contact)) => 1
  }

let compare = (a, b) => switch compareKind(a, b) {
  | 0 =>
  switch compareName(a, b) {
    | 0 => compareAddress(a, b)
    | x => x
  }
  | x => x
}