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

type transferOptions = {
  fee: option(ProtocolXTZ.t),
  gasLimit: option(int),
  storageLimit: option(int),
  parameter: option(string),
  entrypoint: option(string),
};

type commonOptions = {
  fee: option(ProtocolXTZ.t),
  burnCap: option(ProtocolXTZ.t),
  forceLowFee: option(bool),
};

let makeTransferOptions =
    (~fee, ~gasLimit, ~storageLimit, ~parameter, ~entrypoint, ()) => {
  fee,
  gasLimit,
  storageLimit,
  parameter,
  entrypoint,
};

let makeCommonOptions = (~fee, ~burnCap, ~forceLowFee, ()) => {
  fee,
  burnCap,
  forceLowFee,
};

let emptyTransferOptions = {
  fee: None,
  gasLimit: None,
  storageLimit: None,
  parameter: None,
  entrypoint: None,
};

type simulationResults = {
  fee: ProtocolXTZ.t,
  gasLimit: int,
  storageLimit: int,
  revealFee: ProtocolXTZ.t,
};