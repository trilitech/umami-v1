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

include ApiRequest;

open Let;

let useCheckTokenContract = () => {
  let set = (~config, address) =>
    config->NodeAPI.Tokens.checkTokenContract(address);
  ApiRequest.useSetter(~set, ~kind=Logs.Tokens, ~toast=false, ());
};

let useLoadBalance =
    (
      ~requestState,
      ~address: PublicKeyHash.t,
      ~token: PublicKeyHash.t,
      ~kind: TokenRepr.kind,
    ) => {
  let get = (~config, (address, token, kind)) =>
    switch (kind) {
    | TokenRepr.FA1_2 =>
      config->NodeAPI.Tokens.runFA12GetBalance(~address, ~token)
    | FA2(tokenId) =>
      config->NodeAPI.Tokens.callFA2BalanceOf(address, token, tokenId)
    };

  ApiRequest.useLoader(
    ~get,
    ~kind=Logs.Tokens,
    ~requestState,
    (address, token, kind),
  );
};

let useLoadTokens = requestState => {
  let get = (~config as _, ()) =>
    TokensAPI.registeredTokens(`FT)->Promise.value;

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, ());
};

type nftRequest = {
  holder: PublicKeyHash.t,
  allowHidden: bool,
};

let useLoadNFTs = (requestState, request) => {
  let get = (~config as _, request) =>
    TokensAPI.registeredTokens(`NFT((request.holder, request.allowHidden)))
    ->Promise.value;

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, request);
};

type request = {
  account: PublicKeyHash.t,
  index: int,
  numberByAccount: int,
};

type registry = {
  registered: array(TokenRegistry.Cache.token),
  toRegister: array(TokenRegistry.Cache.token),
  nextIndex: int,
};

let useLoadTokensRegistry = (requestState, request) => {
  let get = (~config, request) => {
    let%AwaitMap (registered, toRegister, nextIndex) =
      TokensAPI.fetchAccountsTokensRegistry(
        config,
        ~accounts=[request.account],
        ~index=request.index,
        ~numberByAccount=request.numberByAccount,
      );
    {registered, toRegister, nextIndex};
  };

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, request);
};

type tokens = {
  sorted: TokenRegistry.Cache.t,
  nextIndex: int,
};

let useLoadAccountsTokens = (requestState, request) => {
  let get = (~config, request) => {
    let%AwaitMap (sorted, nextIndex) =
      TokensAPI.fetchAccountsTokens(
        config,
        ~accounts=[request.account],
        ~index=request.index,
        ~numberByAccount=request.numberByAccount,
        ~withFullCache=false,
      );
    {sorted, nextIndex};
  };

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, request);
};

let useLoadAccountsTokensStream = (onTokens, requestState, request) => {
  let rec get = (accumulatedTokens, ~config, request) => {
    Js.log("Requesting tokens:");
    Js.log(request);
    let%Await (sorted, nextIndex) =
      TokensAPI.fetchAccountsTokens(
        config,
        ~accounts=[request.account],
        ~index=request.index,
        ~numberByAccount=request.numberByAccount,
        ~withFullCache=false,
      );
    let accumulatedTokens =
      accumulatedTokens->TokenRegistry.Cache.merge(sorted);
    onTokens(sorted);
    nextIndex <= request.index
      ? Promise.ok({sorted: accumulatedTokens, nextIndex})
      : get(~config, accumulatedTokens, {...request, index: nextIndex});
  };

  ApiRequest.useLoader(
    ~get=get(TokenRegistry.Cache.empty),
    ~kind=Logs.Tokens,
    ~requestState,
    request,
  );
};

let useDelete = (~sideEffect=?, ()) => {
  let set = (~config as _, token) =>
    TokensAPI.removeToken(token, ~pruneCache=true)->Promise.value;

  ApiRequest.useSetter(
    ~logOk=_ => I18n.t#token_deleted,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};

let useCreate = (~sideEffect=?, ()) => {
  let set = (~config, token) => TokensAPI.addFungibleToken(config, token);

  ApiRequest.useSetter(
    ~logOk=_ => I18n.t#token_created,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};

type nfts = {
  tokens: TokenRegistry.Cache.t,
  holder: PublicKeyHash.t,
};

let useRegisterNFTs = (~sideEffect=?, ()) => {
  let set = (~config as _, nfts) =>
    TokensAPI.registerNFTs(nfts.tokens, nfts.holder)->Promise.value;

  ApiRequest.useSetter(
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};
