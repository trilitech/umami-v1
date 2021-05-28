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

open ProtocolOptions;

type currency =
  | XTZ(ProtocolXTZ.t)
  | Token(TokenRepr.Unit.t, TokenRepr.t);

type elt = {
  destination: string,
  amount: currency,
  tx_options: transferOptions,
};

type t = {
  source: string,
  transfers: list(elt),
  common_options: commonOptions,
};

let makeXTZ = t => t->XTZ;
let makeToken = (~amount, ~token) => Token(amount, token);

let currencyToInt64 =
  fun
  | XTZ(xtz) => xtz->ProtocolXTZ.toInt64
  | Token(curr, _) => curr->TokenRepr.Unit.toBigNumber->ReBigNumber.toInt64;

let currencyToBigNumber =
  fun
  | XTZ(xtz) => xtz->ProtocolXTZ.toInt64->ReBigNumber.fromInt64
  | Token(curr, _) => curr->TokenRepr.Unit.toBigNumber;

let currencyToString =
  fun
  | XTZ(xtz) => xtz->ProtocolXTZ.toString
  | Token(curr, _) => curr->TokenRepr.Unit.toNatString;

let getXTZ =
  fun
  | XTZ(xtz) => Some(xtz)
  | _ => None;

let getToken =
  fun
  | Token(t, c) => Some((t, c))
  | _ => None;

let getTokenExn = t => t->getToken->Option.getExn;

let makeSingleTransferElt =
    (
      ~destination,
      ~amount,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) => {
  destination,
  amount,
  tx_options:
    makeTransferOptions(
      ~fee,
      ~gasLimit,
      ~storageLimit,
      ~parameter,
      ~entrypoint,
      (),
    ),
};

let makeSingleXTZTransferElt =
    (
      ~destination,
      ~amount,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) =>
  makeSingleTransferElt(
    ~destination,
    ~amount=makeXTZ(amount),
    ~fee?,
    ~parameter?,
    ~entrypoint?,
    ~gasLimit?,
    ~storageLimit?,
    (),
  );

/* Tokens cannot define parameter and entrypoint, since they are
   already translated as parameters into an entrypoint */
let makeSingleTokenTransferElt =
    (~destination, ~amount, ~token, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) =>
  makeSingleTransferElt(
    ~destination,
    ~amount=makeToken(~amount, ~token),
    ~fee?,
    ~gasLimit?,
    ~storageLimit?,
    (),
  );

let makeTransfers =
    (~source, ~transfers, ~fee=?, ~burnCap=?, ~forceLowFee=?, ()) => {
  let common_options = makeCommonOptions(~fee, ~burnCap, ~forceLowFee, ());
  {source, transfers, common_options};
};
