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

type Errors.t +=
  | NotFA12Contract(string);

let () =
  Errors.registerHandler(
    "Tokens",
    fun
    | NotFA12Contract(_) => I18n.t#error_check_contract->Some
    | _ => None,
  );

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

let tokensStorageKey = "wallet-tokens";

let useLoadTokens = requestState => {
  let get = (~config as _, ()) =>
    LocalStorage.getItem(tokensStorageKey)
    ->Js.Nullable.toOption
    ->Option.mapWithDefault([||], storageString =>
        storageString->Js.Json.parseExn->Token.Decode.array
      )
    ->Array.map(token => {(token.address, token)})
    ->PublicKeyHash.Map.fromArray
    ->Promise.ok;

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, ());
};

let useDelete = (~sideEffect=?, ()) => {
  let set = (~config as _, token) => {
    let tokens =
      LocalStorage.getItem(tokensStorageKey)
      ->Js.Nullable.toOption
      ->Option.mapWithDefault([||], storageString =>
          storageString->Js.Json.parseExn->Token.Decode.array
        );

    LocalStorage.setItem(
      tokensStorageKey,
      tokens
      ->Array.keep(t => t != token)
      ->Token.Encode.array
      ->Js.Json.stringify,
    )
    ->Promise.ok;
  };

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
  let (_checkTokenRequest, checkToken) = useCheckTokenContract();
  let set = (~config as _, token) => {
    let%FRes tokenKind = checkToken(token.TokenRepr.address);

    let%FResMap () =
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
    )
    ->Promise.ok;
  };

  ApiRequest.useSetter(
    ~logOk=_ => I18n.t#token_created,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};
