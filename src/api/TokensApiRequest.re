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

let useLoadFA12Balance =
    (~requestState, ~address: PublicKeyHash.t, ~token: PublicKeyHash.t) => {
  let get = (~config, (address, token)) =>
    config->NodeAPI.Tokens.runFA12GetBalance(~address, ~token);

  ApiRequest.useLoader(
    ~get,
    ~kind=Logs.Tokens,
    ~requestState,
    (address, token),
  );
};

let useLoadTokens = requestState => {
  let get = (~config as _, ()) => TokensAPI.registeredTokens()->Promise.value;

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, ());
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
  let set = (~config, token) => TokensAPI.addToken(config, token);

  ApiRequest.useSetter(
    ~logOk=_ => I18n.t#token_created,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};
