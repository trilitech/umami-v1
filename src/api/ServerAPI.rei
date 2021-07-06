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
        string,
        ~types: array(string)=?,
        ~destination: string=?,
        ~limit: int=?,
        unit
      ) =>
      t;
    let mempool: (AppSettings.t, ~account: string) => t;
    let checkToken: (AppSettings.t, ~contract: string) => t;
    let getTokenBalance:
      (AppSettings.t, ~contract: string, ~account: string) => t;
  };

  module Endpoint: {let delegates: AppSettings.t => t;};

  module External: {let bakingBadBakers: t;};

  /* Fetch URL as a JSON. */
  let get: t => Future.t(Result.t(Js.Json.t, string));
};

/** Mezos requests for mempool operations and classical operations. */
module type Explorer = {
  let getFromMempool:
    (string, AppSettings.t, array(Operation.Read.t)) =>
    Future.t(Result.t(array(Operation.Read.t), string));

  let get:
    (
      AppSettings.t,
      string,
      ~types: array(string)=?,
      ~destination: string=?,
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

/** CSV format and parser */
module CSV: {
  type addressValidityError = [
    | `NotAnAccount
    | `NotAContract
    | ReTaquito.Utils.addressValidityError
  ];

  type customEncodingError =
    | CannotParseAddress(string, addressValidityError)
    | CannotParseContract(string, addressValidityError);

  type error =
    | Parser(TezosClient.CSVParser.error(customEncodingError))
    | UnknownToken(string)
    | NoRows
    | CannotParseTokenAmount(ReBigNumber.t, int, int)
    | CannotParseTezAmount(ReBigNumber.t, int, int);

  type t = list(Transfer.elt);
  /* Public key hash encoding */
  let addr: CSVParser.Encodings.element(string, customEncodingError);
  /* Contract hash encoding */
  let token: CSVParser.Encodings.element(string, customEncodingError);
  /* CSV row encoding */
  let rowEncoding:
    CSVParser.Encodings.row(
      (string, ReBigNumber.t, option(string), option(ReBigNumber.t)),
      customEncodingError,
    );

  let parseCSV:
    (string, ~tokens: Map.String.t(TokenRepr.t)) => result(t, error);

  let handleCSVError: error => string;
};
