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

type addressValidityError = [
  | `No_prefix_matched
  | `Invalid_checksum
  | `Invalid_length
  | `UnknownError(int)
];

[@bs.module "@taquito/utils"]
external validateAddressRaw: string => int = "validateAddress";

[@bs.module "@taquito/utils"]
external validateContractAddressRaw: string => int = "validateContractAddress";

let handleValidity =
  fun
  | 0 => `No_prefix_matched
  | 1 => `Invalid_checksum
  | 2 => `Invalid_length
  | 3 => `Valid
  | n => `UnknownError(n);

let validateAddress = s =>
  switch (s->validateAddressRaw->handleValidity) {
  | `Valid => Ok(`Address)
  | #addressValidityError as err => Error(err)
  };

let validateContractAddress = s =>
  switch (s->validateContractAddressRaw->handleValidity) {
  | `Valid => Ok(`Contract)
  | #addressValidityError as err => Error(err)
  };

let validateAnyAddress = s =>
  switch (s->validateContractAddress) {
  | Ok(`Contract) => Ok(`Contract)
  | Error(_) =>
    switch (s->validateAddress) {
    | Ok(`Address) => Ok(`Address)
    | Error(#addressValidityError as err) => Error(err)
    }
  };
