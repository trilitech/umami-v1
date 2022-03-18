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

type Errors.t +=
  | UnknownNetwork(string)
  | FetchError(string)
  | JsonResponseError(string)
  | JsonError(string);

/** URL generators to access data from the Node or API. */
module URL: {
  type t;

  let fromString: string => t;

  let build_url: (string, list((string, string))) => string;

  module Explorer: {
    let operations:
      (
        ConfigContext.env,
        PublicKeyHash.t,
        ~types: array(string)=?,
        ~destination: PublicKeyHash.t=?,
        ~limit: int=?,
        unit
      ) =>
      t;
    let checkToken: (ConfigContext.env, ~contract: PublicKeyHash.t) => t;
    let accountExists: (ConfigContext.env, ~account: PublicKeyHash.t) => t;

    let tokenRegistry:
      (
        ConfigContext.env,
        ~accountsFilter: list(PublicKeyHash.t)=?,
        ~kinds: list(TokenContract.kind)=?,
        ~limit: int64=?,
        ~index: int64=?,
        unit
      ) =>
      t;
  };

  module Endpoint: {
    let runView: ConfigContext.env => t;

    let fa12GetBalanceInput:
      (
        ~config: ConfigContext.env,
        ~contract: PublicKeyHash.t,
        ~account: PublicKeyHash.t
      ) =>
      Js.Json.t;

    let fa2BalanceOfInput:
      (
        ~config: ConfigContext.env,
        ~contract: PublicKeyHash.t,
        ~account: PublicKeyHash.t,
        ~tokenId: int
      ) =>
      Js.Json.t;
  };

  module External: {
    let bakingBadBakers: t;
    // The API might not be available on custom network, or old networks
    let betterCallDevAccountTokens:
      (
        ~config: ConfigContext.env,
        ~account: PublicKeyHash.t,
        ~contract: PublicKeyHash.t=?,
        ~limit: int=?,
        ~index: int=?,
        ~hideEmpty: bool=?,
        ~sortBy: [ | `Balance | `TokenId]=?,
        unit
      ) =>
      Let.result(t);

    // The request does not return the metadata for now
    let betterCallDevBatchAccounts:
      (~config: ConfigContext.env, ~accounts: array(PublicKeyHash.t)) =>
      Let.result(t);
  };

  /* Fetch URL as a JSON. */
  let get: t => Promise.t(Js.Json.t);

  /* Fetch URL with a JSON payload, as a JSON. */
  let postJson: (t, Js.Json.t) => Promise.t(Js.Json.t);
};

/** Mezos requests for mempool operations and classical operations. */
module type Explorer = {
  let getOperations:
    (
      ConfigContext.env,
      PublicKeyHash.t,
      ~types: array(string)=?,
      ~destination: PublicKeyHash.t=?,
      ~limit: int=?,
      unit
    ) =>
    Promise.t(array(Operation.t));
};

/** This generic version exists only for tests purpose */
module ExplorerMaker:
  (Get: {let get: URL.t => Promise.t(Js.Json.t);}) => Explorer;

module Explorer: Explorer;
