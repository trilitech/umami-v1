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

type t = pri string;

type addressValidityError = [
  | `NotAnImplicit
  | `NotAContract
  | ReTaquitoUtils.addressValidityError
];

type parsingError =
  | CannotParseAddress(string, addressValidityError)
  | CannotParseContract(string, addressValidityError);

let handleValidationError: addressValidityError => string;
let handleParsingError: parsingError => string;

type parsedAddress =
  | Contract(t)
  | Implicit(t);

let build: string => result(t, [> ReTaquitoUtils.addressValidityError]);
let buildAny:
  string => result(parsedAddress, [> ReTaquitoUtils.addressValidityError]);

// Checks if given address is a smart contract address (KT1)
let buildContract: string => result(t, parsingError);

// Checks if given string is an implicit contract (tz*)
let buildImplicit: string => result(t, parsingError);

module Scheme: {
  type t =
    | ED25519
    | SECP256K1
    | P256;

  let toString: t => string;
};

type implicit =
  | TZ1
  | TZ2
  | TZ3;

type kind =
  | Implicit(implicit)
  | KT1;

let implicitFromScheme: Scheme.t => implicit;

let kindToString: kind => string;

module DerivationPath: {let default: string;};

type pkh = t;

module Comparator: Belt.Id.Comparable with type t = pkh;