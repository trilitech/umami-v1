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

type disclaimerVersion = Version.t;

let version = Version.mk(1, 0);

let updateTime = "June 25, 2021";

let lowestBound = Version.mk(~fix=0, 1, 0);

let highestBound = Version.mk(1, max_int);

let checkInBound = version =>
  Version.checkInBound(version, lowestBound, highestBound);

let itemName = "eula-version";

let getAgreedVersion = () =>
  switch (LocalStorage.getItem(itemName)->Js.Nullable.toOption) {
  | Some(v) => v->Version.parse->Result.mapWithDefault(None, v => Some(v))
  | None => None
  };

let sign = () => {
  LocalStorage.setItem(itemName, version->Version.toString);
};

let needSigning = () =>
  switch (getAgreedVersion()) {
  | None => true
  | Some(v) => !checkInBound(v)
  };

let getEula = () => "License agreement placeholder";
