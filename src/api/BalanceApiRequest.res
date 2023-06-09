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
  | EveryBalancesFail
  | BalanceNotFound

let () = Errors.registerHandler("Balance", x =>
  switch x {
  | EveryBalancesFail => I18n.Errors.every_balances_fail->Some
  | BalanceNotFound => I18n.Errors.balance_not_found->Some
  | _ => None
  }
)

let getOne = (balancesRequest, address) =>
  balancesRequest->ApiRequest.flatMap((balances, isDone, t) =>
    switch balances->PublicKeyHash.Map.get(address) {
    | Some(balance) if isDone => ApiRequest.Done(Ok(balance), t)
    | Some(balance) => Loading(Some(balance))
    | None if isDone => Done(Error(BalanceNotFound), t)
    | None => Loading(None)
    }
  )

let useLoadBalances = (~forceFetch=true, ~requestState, addresses: list<PublicKeyHash.t>) => {
  let get = (~config: ConfigContext.env, addresses) =>
    config.network
    ->ServerAPI.Explorer.Tzkt.getBalances(~addresses)
    ->Promise.mapOk(PublicKeyHash.Map.fromArray)
  ApiRequest.useLoader(
    ~get,
    ~condition=addresses => !(addresses->Js.List.isEmpty) && forceFetch,
    ~kind=Logs.Balance,
    ~requestState,
    addresses,
  )
}
