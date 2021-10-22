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

type Errors.t +=
  | NotFA12Contract(string);

let () =
  Errors.registerHandler(
    "Tokens",
    fun
    | NotFA12Contract(_) => I18n.t#error_check_contract->Some
    | _ => None,
  );

let tokensStorageKey = "wallet-tokens";

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

let registeredTokens = () =>
  LocalStorage.getItem(tokensStorageKey)
  ->Js.Nullable.toOption
  ->Option.mapWithDefault([||], storageString =>
      storageString->Js.Json.parseExn->Token.Decode.array
    )
  ->Array.map(token => {(token.address, token)})
  ->PublicKeyHash.Map.fromArray
  ->Result.Ok
  ->Promise.value;

let addToken = (config, token) => {
  let%Await tokenKind =
    config->NodeAPI.Tokens.checkTokenContract(token.TokenRepr.address);

  let%AwaitMap () =
    tokenKind == `KFA1_2
      ? Promise.ok()
      : Promise.err(NotFA12Contract((token.TokenRepr.address :> string)));

  let tokens =
    LocalStorage.getItem(tokensStorageKey)
    ->Js.Nullable.toOption
    ->Option.mapWithDefault([||], storageString =>
        storageString->Js.Json.parseExn->Token.Decode.array
      );

  LocalStorage.setItem(
    tokensStorageKey,
    tokens->Array.concat([|token|])->Token.Encode.array->Js.Json.stringify,
  );
};

let removeToken = token => {
  let tokens =
    LocalStorage.getItem(tokensStorageKey)
    ->Js.Nullable.toOption
    ->Option.mapWithDefault([||], storageString =>
        storageString->Js.Json.parseExn->Token.Decode.array
      );

  LocalStorage.setItem(
    tokensStorageKey,
    tokens->Array.keep(t => t != token)->Token.Encode.array->Js.Json.stringify,
  )
  ->Result.Ok
  ->Promise.value;
};
