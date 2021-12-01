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
  | NoTzip16Metadata(PublicKeyHash.t)
  | NoTzip12Metadata(PublicKeyHash.t)
  | TokenIdNotFound(PublicKeyHash.t, int);

let toolkit: ConfigContext.env => ReTaquito.Toolkit.toolkit;

module Tzip16: {
  let makeContract:
    (ReTaquito.Toolkit.toolkit, PublicKeyHash.t) =>
    Promise.t(ReTaquitoContracts.Tzip16Contract.t);

  let read:
    ReTaquitoContracts.Tzip16Contract.t =>
    Promise.t(ReTaquitoTypes.Tzip16.metadataRes);
};

module Tzip12: {
  module Storage: {
    let read:
      ReTaquitoContracts.Tzip12Tzip16Contract.t =>
      Promise.t(ReTaquitoTypes.Tzip12Storage.storage);

    let getToken:
      (PublicKeyHash.t, ReTaquitoTypes.Tzip12Storage.storage, int) =>
      Promise.t(ReTaquitoTypes.Tzip12.metadata);
  };

  let makeContract:
    (ReTaquito.Toolkit.toolkit, PublicKeyHash.t) =>
    Promise.t(ReTaquitoContracts.Tzip12Tzip16Contract.t);

  let read:
    (ReTaquitoContracts.Tzip12Tzip16Contract.t, int) =>
    Promise.t(ReTaquitoTypes.Tzip12.metadata);

  let readContractMetadata:
    ReTaquitoContracts.Tzip12Tzip16Contract.t =>
    Promise.t(ReTaquitoTypes.Tzip16.metadataRes);
};
