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
  // get all multisigs that concern provided addresses
  let getMultisigsConcerningAddresses = (~network, ~addresses) => {
    network->Multisig.API.getAddresses(~addresses)->Promise.flatMapOk(network->Multisig.API.get)
  }

  let get = (~config) =>
    HDWalletAPI.Accounts.get(~config)->Promise.flatMapOk(accounts =>
      getMultisigsConcerningAddresses(
        ~network=config.network,
        ~addresses=accounts->Array.map(account => account.address),
      )
    )

  //  get all pending operations for a multisig contract address
  let getPendingOperations = (~network: Network.t, ~address) =>
    network
    ->Multisig.API.getStorage(~contract=address)
    ->Promise.flatMapOk(storage =>
      network->Multisig.API.getPendingOperations(~bigmap=storage.pendingOps)
    )
}

/* Get */

let useLoad = requestState => {
  let get = (~config: ConfigContext.env, ()) => Base.get(~config)
  ApiRequest.useLoader(~get, ~kind=Logs.Multisig, ~requestState, ())
}

/* Set */

let useUpdate = message => {
  let kind = Logs.Multisig
  let logOk = _ => message//creation ? I18n.multisig_originated : I18n.multisig_updated

  let set = (~config, multisig) => {
    Base.get(~config)->Promise.mapOk(cache =>
      cache->PublicKeyHash.Map.set(multisig.Multisig.address, multisig)->Multisig.Cache.set
    )
  }

  ApiRequest.useSetter(~logOk, ~set, ~kind)
}

/* Delete */

let useDelete = {
  let set = (~config as _, contract) => Multisig.API.removeFromCache(contract)->Promise.value
  let logOk = _ => I18n.contract_removed_from_cache
  let kind = Logs.Multisig
  ApiRequest.useSetter(~logOk, ~set, ~kind)
}

/* Other */

let usePendingOperations = (~requestState, ~address) => {
  let get = (~config: ConfigContext.env, ()) =>
    Base.getPendingOperations(~network=config.network, ~address)
  ApiRequest.useLoader(~get, ~kind=Logs.Multisig, ~requestState, ())
}
