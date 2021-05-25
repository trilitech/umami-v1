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

open UmamiCommon;
include ApiRequest;
module Error = API.Error;
module API = API.Tokens(API.TezosExplorer);

type injection = {
  operation: Token.operation,
  password: string,
};

let useCheckTokenContract = () => {
  let set = (~settings, address) =>
    settings->API.checkTokenContract(address);
  ApiRequest.useSetter(~set, ~kind=Logs.Tokens, ~toast=false, ());
};

let useLoadOperationOffline =
    (
      ~requestState as (request, setRequest),
      ~operation: option(Token.operation),
    ) => {
  let get = (~settings, operation) =>
    settings
    ->API.callGetOperationOffline(operation)
    ->Future.mapError(Error.fromApiToString);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Tokens, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect3(
    () => {
      let shouldReload = ApiRequest.conditionToLoad(request, isMounted);
      operation->Lib.Option.iter(operation =>
        if (shouldReload) {
          getRequest(operation)->ignore;
        }
      );
      None;
    },
    (isMounted, request, operation),
  );

  request;
};

let tokensStorageKey = "wallet-tokens";

let useLoadTokens = requestState => {
  let get = (~settings as _, ()) =>
    LocalStorage.getItem(tokensStorageKey)
    ->Js.Nullable.toOption
    ->Option.mapWithDefault([||], storageString =>
        storageString->Js.Json.parseExn->Token.Decode.array
      )
    ->Array.map(token => {(token.address, token)})
    ->Map.String.fromArray
    ->Result.Ok
    ->Future.value;

  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, ());
};

let useDelete = (~sideEffect=?, ()) => {
  let set = (~settings as _, token) => {
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
    ->Result.Ok
    ->Future.value;
  };

  ApiRequest.useSetter(
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};

let useCreate = (~sideEffect=?, ()) => {
  let set = (~settings as _, token) => {
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
    ->Result.Ok
    ->Future.value;
  };

  ApiRequest.useSetter(
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};
