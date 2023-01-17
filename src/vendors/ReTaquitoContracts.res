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

module Types = ReTaquitoTypes

module BigNumber = Types.BigNumber

@ocaml.doc(" Bindings of contracts metadata extensions ")
module Extension = {
  include Types.Extension

  @module("@taquito/tzip12") @new
  external tzip12Module: unit => t = "Tzip12Module"

  @module("@taquito/tzip16") @new
  external tzip16Module: unit => t = "Tzip16Module"

  @module("@taquito/tzip12") external tzip12: abstraction = "tzip12"

  @module("@taquito/tzip16") external tzip16: abstraction = "tzip16"

  @module("@taquito/taquito")
  external compose: (abstraction, abstraction) => abstraction = "compose"
}

module Contract = (Abstraction: Types.ContractAbstraction) => {
  include Abstraction

  @send
  external at: (
    Types.Toolkit.contract,
    PublicKeyHash.t,
    option<Extension.abstraction>,
  ) => Js.Promise.t<t> = "at"

  let at = (~extension=?, contract, pkh) => at(contract, pkh, extension)
}

module Tzip16Contract = {
  include Contract(Types.Tzip16Contract)

  let at = (contract, pkh) => at(contract, pkh, ~extension=Extension.tzip16)
}

@ocaml.doc(" Generic token contract, with Tzip12 metadata and Tzip16 metadata ")
module Tzip12Tzip16Contract = {
  include Contract(Types.Tzip12Tzip16Contract)

  let at = (contract, pkh) =>
    at(
      contract,
      pkh,
      ~extension={
        open Extension
        compose(tzip12, tzip16)
      },
    )
}

@ocaml.doc(" FA12 contracts specific interface ")
module FA12 = {
  include Contract(Types.FA12)

  let at = (contract, pkh) =>
    at(
      contract,
      pkh,
      ~extension={
        open Extension
        compose(tzip12, tzip16)
      },
    )

  let transfer = (c: t, src, dst, amount) => (c->Types.FA12.methods).transfer(. src, dst, amount)
}

@ocaml.doc(" FA12 contracts specific interface ")
module FA2 = {
  include Contract(Types.FA2)

  let at = (contract, pkh) =>
    at(
      contract,
      pkh,
      ~extension={
        open Extension
        compose(tzip12, tzip16)
      },
    )

  let transfer = (c: t, txs) => (c->Types.FA2.methods).transfer(. txs)
}

@ocaml.doc(" Multisig contracts specific interface")
module Multisig = {
  include Contract(Types.Multisig)

  let propose = (c: t, f) => (c->Types.Multisig.methods).propose(. f)
  let approve = (c: t, o) => (c->Types.Multisig.methods).approve(. o)
  let execute = (c: t, o) => (c->Types.Multisig.methods).execute(. o)
}