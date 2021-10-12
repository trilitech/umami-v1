/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

/* open UmamiCommon; */
open Let;
open TokenRegistry;

type Errors.t +=
  | NotFAContract(string);

let () =
  Errors.registerHandler(
    "Tokens",
    fun
    | NotFAContract(_) => I18n.t#error_check_contract->Some
    | _ => None,
  );

module BetterCallDev = {
  open BCD;

  // Returns a list of arrays: this will be treated later, so we return it as raw as possible for now
  let fetchTokens = (config, alreadyFetched, account) => {
    let rec fetch = (alreadyFetched, index, account) => {
      let%Await url =
        ServerAPI.URL.External.betterCallDevAccountTokens(
          ~config,
          ~account,
          ~sortBy=`TokenId,
          ~limit=20,
          ~index,
          (),
        )
        ->Promise.value;
      let%Await json = url->ServerAPI.URL.get;
      let%Await tokens = json->JsonEx.decode(Decode.decoder)->Promise.value;
      let alreadyFetched = [tokens.balances, ...alreadyFetched];
      index * 20 < tokens.total
        ? fetch(alreadyFetched, index + 1, account)
        : alreadyFetched->Promise.ok;
    };
    fetch(alreadyFetched, 0, account);
  };

  let fetchAccountsTokens = (config, accounts) => {
    let rec fetch = (alreadyFetched, accounts) => {
      switch (accounts) {
      | [] => alreadyFetched->Promise.ok
      | [account, ...accounts] =>
        let%Await fetched = fetchTokens(config, alreadyFetched, account);
        fetch(fetched, accounts);
      };
    };
    fetch([], accounts);
  };

  // Returns a list of arrays: this will be treated later, so we return it as raw as possible for now
  let _fetchTokensBatchAccounts = (config, accounts) => {
    // The request accepts 10 accounts at most
    let rec fetch = (alreadyFetched, offset) =>
      if (offset >= accounts->Array.length) {
        alreadyFetched->Promise.ok;
      } else {
        let%Await url =
          ServerAPI.URL.External.betterCallDevBatchAccounts(
            ~config,
            ~accounts=accounts->Array.slice(~offset, ~len=10),
          )
          ->Promise.value;
        let%Await json = url->ServerAPI.URL.get;
        let%Await tokens = json->JsonEx.decode(Decode.decoder)->Promise.value;
        fetch([tokens.balances, ...alreadyFetched], offset + 10);
      };
    fetch([], 0);
  };
};

/* From a list of tokens and the cache, reconstructs the list of tokens with
   their metadata the user has registered */
let unfoldRegistered = (tokens, cache: Cache.t) => {
  let merge = (_, registered, tokens) => {
    switch (registered, tokens) {
    | (None, _)
    // What should we do if the token is not in cache?
    | (_, None) => None
    | (Some(registered), Some(contract)) =>
      let tokens =
        contract.Cache.tokens
        ->Map.Int.keep((id, _) =>
            registered.Registered.tokens->Set.Int.has(id)
          );
      Some({...contract, tokens});
    };
  };
  PublicKeyHash.Map.merge(tokens, cache, merge);
};

let registeredTokens = () => {
  let%Res tokens = Registered.get();
  let%ResMap cache = Cache.get();
  tokens->unfoldRegistered(cache);
};

let addTokenToCache = (config, token: Token.t) => {
  let address = token.address;
  let%Await tokenKind = config->NodeAPI.Tokens.checkTokenContract(address);

  let%AwaitMap () =
    switch (tokenKind) {
    | #TokenContract.kind => Promise.ok()
    | _ => Promise.err(NotFAContract((address :> string)))
    };

  let tokens =
    Cache.get()
    ->Result.getWithDefault(PublicKeyHash.Map.empty)
    ->Cache.addToken(Full(token));

  Cache.set(tokens);
};

let addTokenToRegistered = (token: Token.t) => {
  let tokens =
    Registered.get()
    ->Result.getWithDefault(PublicKeyHash.Map.empty)
    ->Registered.registerToken(token);

  Registered.set(tokens);
};

let addToken = (config, token: Token.t) => {
  let%AwaitMap () = addTokenToCache(config, token);
  addTokenToRegistered(token);
};

let removeFromCache = token => {
  let%ResMap tokens = Cache.get();
  tokens->Cache.removeToken(token)->Cache.set;
};

let removeFromRegistered = (token: Token.t) => {
  let%ResMap tokens = Registered.get();
  Registered.set(
    tokens->Registered.removeToken(token.address, TokenRepr.id(token)),
  );
};

let removeToken = (token, ~pruneCache) => {
  let%Res () = pruneCache ? removeFromCache(Full(token)) : Ok();
  removeFromRegistered(token);
};

let fetchTokenContracts = (config, ~accounts, ~kinds, ~limit, ~index) => {
  open ServerAPI;

  let%Await tokens =
    URL.Explorer.tokenRegistry(
      config,
      ~accountsFilter=accounts,
      ~kinds,
      ~limit,
      ~index,
      (),
    )
    ->URL.get;

  tokens->JsonEx.decode(TokenContract.Decode.array)->Promise.value;
};

let fetchTokenRegistry = (config, ~kinds, ~limit, ~index) =>
  fetchTokenContracts(config, ~accounts=[], ~kinds, ~limit, ~index);
