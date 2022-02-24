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

type t =
  | Version(int, int, option(int), option(string));

type Errors.t +=
  | VersionFormat(string)
  | UnknownVersion({
      current: t,
      expected: t,
    })
  | VersionNotInBound({
      lowestBound: t,
      highestBound: t,
      version: t,
    });

let toString = (Version(major, minor, fix, patch)) =>
  Int.toString(major)
  ++ "."
  ++ Int.toString(minor)
  ++ fix
     ->Option.map(fix => "." ++ Int.toString(fix))
     ->Option.getWithDefault("")
  ++ patch->Option.map(patch => "~" ++ patch)->Option.getWithDefault("");

let () =
  Errors.registerHandler(
    "Version",
    fun
    | VersionFormat(s) => I18n.Errors.version_format(s)->Some
    | UnknownVersion({current, expected}) =>
      I18n.Errors.unknown_version(current->toString, expected->toString)->Some
    | VersionNotInBound({lowestBound, highestBound, version}) =>
      I18n.Errors.version_not_in_bound(
        lowestBound->toString,
        highestBound->toString,
        version->toString,
      )
      ->Some
    | _ => None,
  );

let mk = (~fix=?, ~patch=?, major, minor) =>
  Version(major, minor, fix, patch);

let compareFix =
  fun
  | (None, None) => 0
  | (Some(_), None) => (-1)
  | (None, Some(_)) => 1
  | (Some(fix1), Some(fix2)) => compare(fix1, fix2);

let compare =
    (Version(major1, minor1, fix1, _), Version(major2, minor2, fix2, _)) => {
  let major = compare(major1, major2);
  let minor = compare(minor1, minor2);
  let fix = compareFix((fix1, fix2));
  major != 0 ? major : minor != 0 ? minor : fix;
};

module Comparable =
  Id.MakeComparable({
    type nonrec t = t;
    let cmp = compare;
  });

let leqFix =
  fun
  | (_, None) => true
  | (None, Some(_)) => false
  | (Some(fix1), Some(fix2)) => fix1 <= fix2;

let leqVersion =
    (Version(major1, minor1, fix1, _), Version(major2, minor2, fix2, _)) =>
  major1 <= major2 && minor1 <= minor2 && leqFix((fix1, fix2));

let checkInBound = (version, lowestBound, highestBound) =>
  leqVersion(lowestBound, version) && leqVersion(version, highestBound);

let checkInBoundRes = (~version, ~lowestBound, ~highestBound) =>
  checkInBound(version, lowestBound, highestBound)
    ? Ok() : Error(VersionNotInBound({version, lowestBound, highestBound}));

let checkVersion = (~current, ~expected) =>
  checkInBound(current, expected, expected)
    ? Ok() : Error(UnknownVersion({current, expected}));

let parse = version => {
  let parseInt = value =>
    value->Int.fromString->Result.fromOption(VersionFormat(version));

  // parse a value of the form "<int>~patch", where ~patch is optional
  let parseFixAndPatch = value =>
    switch (value->Js.String2.splitAtMost(~limit=1, "~")) {
    | [|fix|] => fix->parseInt->Result.map(fix => (fix, None))
    | [|fix, patch|] => fix->parseInt->Result.map(fix => (fix, Some(patch)))
    | _ => Error(VersionFormat(version))
    };

  switch (version->Js.String2.split(".")) {
  | [|major, minor|] => Result.map2(major->parseInt, minor->parseInt, mk)

  | [|major, minor, fixAndPatch|] =>
    fixAndPatch
    ->parseFixAndPatch
    ->Result.flatMap(((fix, patch)) =>
        Result.map2(major->parseInt, minor->parseInt, mk(~fix, ~patch?))
      )

  | _ => Error(VersionFormat(version))
  };
};
