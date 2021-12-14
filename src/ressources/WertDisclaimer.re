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

let version = Version.mk(1, 0);

let lowestBound = Version.mk(~fix=0, 1, 0);

let highestBound = Version.mk(1, max_int);

let checkInBound = version =>
  Version.checkInBound(version, lowestBound, highestBound);

module Value = {
  type t = Version.t;
  let key = "wert-eula-version";

  let encoder = v => Json.Encode.(v->Version.toString->string);
  let decoder = json => {
    switch (json->Json.Decode.string->Version.parse) {
    | Ok(v) => v
    | Error(e) => JsonEx.(raise(InternalError(e)))
    };
  };
};

module Storage = LocalStorage.Make(Value);

let getAgreedVersion = () =>
  switch (Storage.get()) {
  | Ok(v) => Some(v)
  | Error(_) => None
  };

let sign = () => {
  Storage.set(version);
};

let needSigning = () =>
  switch (getAgreedVersion()) {
  | None => true
  | Some(v) => !checkInBound(v)
  };

let getEula = () => "License agreement placeholder";