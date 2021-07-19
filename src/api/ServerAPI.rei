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

/** URL generators to access data from the Node or API. */
module URL: {
  type t;

  let fromString: string => t;

  module Explorer: {
    let operations:
      (
        AppSettings.t,
        PublicKeyHash.t,
        ~types: array(string)=?,
        ~destination: PublicKeyHash.t=?,
        ~limit: int=?,
        unit
      ) =>
      t;
    let mempool: (AppSettings.t, ~account: PublicKeyHash.t) => t;
    let checkToken: (AppSettings.t, ~contract: PublicKeyHash.t) => t;
    let getTokenBalance:
      (AppSettings.t, ~contract: PublicKeyHash.t, ~account: PublicKeyHash.t) =>
      t;
  };

  module Endpoint: {let delegates: AppSettings.t => t;};

  module External: {let bakingBadBakers: t;};

  /* Fetch URL as a JSON. */
  let get: t => Future.t(Result.t(Js.Json.t, string));
};

/** Mezos requests for mempool operations and classical operations. */
module type Explorer = {
  let getFromMempool:
    (PublicKeyHash.t, AppSettings.t, array(Operation.Read.t)) =>
    Future.t(Result.t(array(Operation.Read.t), string));

  let getOperations:
    (
      AppSettings.t,
      PublicKeyHash.t,
      ~types: array(string)=?,
      ~destination: PublicKeyHash.t=?,
      ~limit: int=?,
      ~mempool: bool=?,
      unit
    ) =>
    Future.t(Result.t(array(Operation.Read.t), string));
};

/** This generic version exists only for tests purpose */
module ExplorerMaker:
  (Get: {let get: URL.t => Future.t(Result.t(Js.Json.t, string));}) =>
   Explorer;

module Explorer: Explorer;
