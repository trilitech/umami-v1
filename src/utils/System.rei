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
  | NoSuchFileError(string);

let getVersion: unit => string;

type plateform = [ | `darwin | `win32 | `linux];

let plateform: plateform;
let isMac: bool;

let openExternal: string => unit;

module Path: {
  type t;

  let mk: string => t;

  let toString: t => string;

  let join: array(t) => t;

  module Ops: {
    let (!): string => t;

    let (/): (t, t) => t;
  };

  let getCurrent: unit => t;
  let getAppData: unit => t;
};

let homeDir: unit => Path.t;
let appDir: unit => Path.t;

module File: {
  type encoding =
    | Utf8
    | Raw(string);

  let read: (~encoding: encoding=?, Path.t) => Promise.t(string);

  let write:
    (~encoding: encoding=?, ~name: Path.t, string) => Promise.t(unit);

  module CopyMode: {
    type t;
    let copy_excl: t;
    let copy_ficlone: t;
    let copy_ficlone_force: t;

    let assemble: (t, t) => t;
  };

  let access: Path.t => FutureBase.t(bool);

  let copy:
    (~name: Path.t, ~dest: Path.t, ~mode: CopyMode.t) => Promise.t(unit);

  let rm: (~name: Path.t) => Promise.t(unit);

  let initIfNotExists:
    (~encoding: encoding=?, ~path: Path.t, string) => Promise.t(unit);
  let initDirIfNotExists: Path.t => Promise.t(unit);
};

module Client: {
  let resetDir: Path.t => Promise.t(unit);

  let initDir: Path.t => Promise.t(unit);
};

let reload: unit => unit;
