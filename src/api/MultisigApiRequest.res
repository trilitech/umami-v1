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

/* MULTISIG */

module Base = {
  let get = (~config) =>
    HDWalletAPI.Accounts.get(~config)->Promise.flatMapOk(accounts =>
      config.network
      ->Multisig.API.getAddresses(
        ~addresses=accounts->Array.map(account => account.address),
        ~contract=config.network.chain->Multisig.contract,
      )
      ->Promise.flatMapOk(config.network->Multisig.API.get)
    )

  let getPendingOperations = (~config: ConfigContext.env, ~address) =>
    config.network
    ->Multisig.API.getStorage(~contract=address)
    ->Promise.flatMapOk(storage =>
      config.network->Multisig.API.getPendingOperations(~bigmap=storage.pendingOps)
    )
}

/* Get */

let useLoad = requestState => {
  let get = (~config: ConfigContext.env, ()) => Base.get(~config)
  ApiRequest.useLoader(~get, ~kind=Logs.Multisig, ~requestState, ())
}

/* Set */

let useUpdate = {
  let logOk = _ => I18n.contract_updated
  let kind = Logs.Multisig
  let set = (~config, multisig) => {
    Base.get(~config)->Promise.mapOk(cache =>
      cache->PublicKeyHash.Map.set(multisig.Multisig.address, multisig)->Multisig.Cache.set
    )
  }
  ApiRequest.useSetter(~logOk, ~set, ~kind)
}

/* Other */

let usePendingOperations = (~requestState, ~address) => {
  let get = (~config: ConfigContext.env, ()) => Base.getPendingOperations(~config, ~address)
  ApiRequest.useLoader(~get, ~kind=Logs.Multisig, ~requestState, ())
}
