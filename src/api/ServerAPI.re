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

module Path = {
  module Endpoint = {
    let delegates = "/chains/main/blocks/head/context/delegates\\?active=true";
  };
  module API = {
    let mempool_operations = "mempool/accounts";
  };
};

module URL = {
  type t = string;

  let arg_opt = (v, n, f) => v->Option.map(a => (n, f(a)));

  let build_args = l =>
    l->List.map(((a, v)) => a ++ "=" ++ v)->List.toArray
    |> Js.Array.joinWith("&");

  let build_explorer_url = (network, path, args) => {
    AppSettings.explorer(network)
    ++ "/"
    ++ path
    ++ (args == [] ? "" : "?" ++ args->build_args);
  };

  let fromString = s => s;

  let get = url =>
    url
    ->Fetch.fetch
    ->FutureJs.fromPromise(Js.String.make)
    ->Future.flatMapOk(response =>
        response->Fetch.Response.json->FutureJs.fromPromise(Js.String.make)
      );

  module Explorer = {
    let operations =
        (
          settings: AppSettings.t,
          account,
          ~types: option(array(string))=?,
          ~destination: option(string)=?,
          ~limit: option(int)=?,
          (),
        ) => {
      let operationsPath = "accounts/" ++ account ++ "/operations";
      let args =
        Lib.List.(
          []
          ->addOpt(destination->arg_opt("destination", dst => dst))
          ->addOpt(limit->arg_opt("limit", lim => lim->Js.Int.toString))
          ->addOpt(types->arg_opt("types", t => t->Js.Array2.joinWith(",")))
        );
      let url = build_explorer_url(settings, operationsPath, args);
      url;
    };

    let mempool = (network, ~account) => {
      let path =
        Path.API.mempool_operations ++ "/" ++ account ++ "/operations";
      build_explorer_url(network, path, []);
    };

    let checkToken = (network, ~contract) => {
      let path = "tokens/exists/" ++ contract;
      build_explorer_url(network, path, []);
    };

    let getTokenBalance = (network, ~contract, ~account) => {
      let path =
        "accounts/" ++ account ++ "/tokens/" ++ contract ++ "/balance";
      build_explorer_url(network, path, []);
    };
  };

  module Endpoint = {
    let delegates = settings =>
      AppSettings.endpoint(settings) ++ Path.Endpoint.delegates;
  };

  module External = {
    let bakingBadBakers = "https://api.baking-bad.org/v2/bakers";
  };
};

let tryMap = (result: Result.t('a, string), transform: 'a => 'b) =>
  try(
    switch (result) {
    | Ok(value) => Ok(transform(value))
    | Error(error) => Error(error)
    }
  ) {
  | Json.ParseError(error) => Error(error)
  | Json.Decode.DecodeError(error) => Error(error)
  | _ => Error("Unknown error")
  };

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

module ExplorerMaker =
       (Get: {let get: string => Future.t(Result.t(Js.Json.t, string));}) => {
  let getFromMempool = (account, network, operations) =>
    network
    ->URL.Explorer.mempool(~account)
    ->Get.get
    ->Future.map(result =>
        result->tryMap(x =>
          (
            operations,
            x |> Json.Decode.(array(Operation.Read.decodeFromMempool)),
          )
        )
      )
    >>= (
      ((operations, mempool)) => {
        module Comparator = Operation.Read.Comparator;
        let operations =
          Set.fromArray(operations, ~id=(module Operation.Read.Comparator));

        let operations =
          mempool->Array.reduce(operations, Set.add)->Set.toArray;

        Future.value(Ok(operations));
      }
    );

  let get =
      (
        network,
        account,
        ~types: option(array(string))=?,
        ~destination: option(string)=?,
        ~limit: option(int)=?,
        ~mempool: bool=false,
        (),
      ) =>
    network
    ->URL.Explorer.operations(account, ~types?, ~destination?, ~limit?, ())
    ->Get.get
    ->Future.map(result =>
        result->tryMap(Json.Decode.(array(Operation.Read.decode)))
      )
    >>= (
      operations =>
        mempool
          ? getFromMempool(account, network, operations)
          : Future.value(Ok(operations))
    );
};

module Explorer = ExplorerMaker(URL);

module CSV = {
  open CSVParser;

  type addressValidityError = [
    | `NotAnAccount
    | `NotAContract
    | ReTaquito.Utils.addressValidityError
  ];

  type customEncodingError =
    | CannotParseAddress(string, addressValidityError)
    | CannotParseContract(string, addressValidityError);

  type error =
    | Parser(CSVParser.error(customEncodingError))
    | UnknownToken(string)
    | NoRows
    | CannotParseTokenAmount(ReBigNumber.t, int, int)
    | CannotParseTezAmount(ReBigNumber.t, int, int);

  type t = list(Transfer.elt);

  let checkAddress = a => {
    switch (ReTaquito.Utils.validateAnyAddress(a)) {
    | Ok(`Address) => Ok(a)
    | Ok(`Contract) => Error(CannotParseAddress(a, `NotAnAccount))
    | Error(#addressValidityError as err) =>
      Error(CannotParseAddress(a, err))
    };
  };

  let checkContract = a => {
    switch (ReTaquito.Utils.validateAnyAddress(a)) {
    | Ok(`Contract) => Ok(a)
    | Ok(`Address) => Error(CannotParseContract(a, `NotAContract))
    | Error(#addressValidityError as err) =>
      Error(CannotParseContract(a, err))
    };
  };

  let addr = Encodings.custom(~conv=checkAddress);
  let token = Encodings.custom(~conv=checkContract);

  let rowEncoding =
    Encodings.(mkRow(tup4(addr, number, opt(token), opt(number))));

  let handleTezRow = (index, destination, amount) =>
    amount
    ->ReBigNumber.toString
    ->Tez.fromString
    ->ResultEx.fromOption(Error(CannotParseTezAmount(amount, index, 2)))
    ->Result.map(amount =>
        Transfer.makeSingleTezTransferElt(~destination, ~amount, ())
      );

  let handleTokenRow = (tokens, index, destination, amount, token) =>
    tokens
    ->Map.String.get(token)
    ->Option.mapWithDefault(Error(UnknownToken(token)), token =>
        amount
        ->Token.Unit.fromBigNumber
        ->ResultEx.fromOption(
            Error(CannotParseTokenAmount(amount, index, 2)),
          )
        ->Result.map(amount =>
            Transfer.makeSingleTokenTransferElt(
              ~destination,
              ~amount,
              ~token,
              (),
            )
          )
      );

  let handleRow = (tokens, index, row) =>
    switch (row) {
    | (destination, amount, None, _) =>
      handleTezRow(index, destination, amount)
    | (destination, amount, Some(token), _) =>
      handleTokenRow(tokens, index, destination, amount, token)
    };

  let handleCSV = (rows, tokens) =>
    rows->List.mapWithIndex(handleRow(tokens))->ResultEx.collect;

  let parseCSV = (content, ~tokens) => {
    let rows =
      parseCSV(content, rowEncoding)
      ->ResultEx.mapError(e => Error(Parser(e)));
    switch (rows) {
    | Ok([]) => Error(NoRows)
    | Ok(rows) => handleCSV(rows, tokens)
    | Error(e) => Error(e)
    };
  };

  let handleAddressValidationError: addressValidityError => string =
    fun
    | `NotAnAccount => I18n.taquito#not_an_account
    | `NotAContract => I18n.taquito#not_a_contract
    | `No_prefix_matched => I18n.taquito#no_prefix_matched
    | `Invalid_checksum => I18n.taquito#invalid_checksum
    | `Invalid_length => I18n.taquito#invalid_length
    | `UnknownError(n) => I18n.taquito#unknown_error_code(n);

  let handleCustomError =
    fun
    | CannotParseAddress(a, r) =>
      I18n.csv#cannot_parse_address(a, handleAddressValidationError(r))
    | CannotParseContract(a, r) =>
      I18n.csv#cannot_parse_contract(a, handleAddressValidationError(r));

  let handleCSVError = e =>
    e->CSVParser.(
         fun
         | Parser(CannotParseNumber(row, col)) =>
           I18n.csv#cannot_parse_number(row + 1, col + 1)
         | Parser(CannotParseBool(row, col)) =>
           I18n.csv#cannot_parse_boolean(row + 1, col + 1)
         | Parser(CannotParseCustomValue(e, row, col)) =>
           I18n.csv#cannot_parse_custom_value(
             handleCustomError(e),
             row + 1,
             col + 1,
           )
         | Parser(CannotParseRow(row)) => I18n.csv#cannot_parse_row(row + 1)
         | Parser(CannotParseCSV) => I18n.csv#cannot_parse_csv
         | NoRows => I18n.csv#no_rows
         | CannotParseTokenAmount(v, row, col) =>
           I18n.csv#cannot_parse_token_amount(v, row + 1, col + 1)
         | CannotParseTezAmount(v, row, col) =>
           I18n.csv#cannot_parse_tez_amount(v, row + 1, col + 1)
         | UnknownToken(s) => I18n.csv#unknown_token(s)
       );
};
