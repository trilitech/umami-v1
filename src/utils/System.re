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

%raw
"
var electron = require('electron');
var { app, Menu, MenuItem } = electron.remote;
var shell = electron.shell;
var process = require('process');
var OS = require('os');
var fs = require('fs');
var path = require('path');

";

type Errors.t +=
  | NoSuchFileError(string);

[@bs.scope "app"] [@bs.val] external getVersion: unit => string = "getVersion";
[@bs.scope "app"] [@bs.val] external getName: unit => string = "getName";

[@bs.scope "shell"] [@bs.val]
external openExternal: string => Js.Promise.t(unit) = "openExternal";
let openExternal = url => url->openExternal->ignore;

type plateform = [ | `darwin | `win32 | `linux];

[@bs.scope "process"] [@bs.val] external plateform: plateform = "platform";

let isMac = plateform == `darwin;

[@bs.scope "process.env"] [@bs.val]
external nodeEnv: Js.Nullable.t(string) = "NODE_ENV";

[@bs.scope "process.env"] [@bs.val]
external appImage: Js.Nullable.t(string) = "APPIMAGE";

let isDev =
  nodeEnv
  ->Js.Nullable.toOption
  ->Option.mapWithDefault(false, a => a == "development");

[@bs.val] external window: 'a = "window";

let reload = () => window##location##reload();

let hasAutoUpdate = () => {
  !isDev
  && (
    switch (plateform) {
    | `darwin => true
    | `win32 => true
    | `linux => !appImage->Js.Nullable.isNullable
    }
  );
};

module Path: {
  type t = pri string;

  let mk: string => t;

  let toString: t => string;

  let join: array(t) => t;

  let getCurrent: unit => t;
  let getAppData: unit => t;
  module Ops: {
    let (!): string => t;
    let (/): (t, t) => t;
  };
} = {
  type t = string;

  let mk = p => p;

  let toString = p => p;

  [@bs.scope "app"] [@bs.val] external getCurrent: unit => t = "getAppPath";

  [@bs.scope "app"] [@bs.val]
  external getAppData: ([@bs.as "appData"] _, unit) => t = "getPath";

  [@bs.module "path"] [@bs.variadic]
  external join: array(t) => string = "join";

  module Ops = {
    let (!) = a => mk(a);
    let (/) = (a, b) => join([|a, b|]);
  };
};

let appDir = () => Path.Ops.(Path.getAppData() / (!getName()));

module File = {
  open Path.Ops;

  let no_such_file = "no such file";

  let parseError = (e: RawJsError.t) =>
    switch (e.message) {
    | s when s->Js.String2.includes(no_such_file) => NoSuchFileError(s)
    | s => Errors.Generic(s)
    };

  let () =
    Errors.registerHandler(
      "System",
      fun
      | NoSuchFileError(s) => s->Some
      | _ => None,
    );

  type encoding =
    | Utf8
    | Raw(string);

  let string_of_encoding = e =>
    switch (e) {
    | Utf8 => "utf8"
    | Raw(s) => s
    };

  [@bs.scope "fs"] [@bs.val]
  external readFile:
    (
      ~name: Path.t,
      ~encoding: string,
      (Js.Nullable.t(RawJsError.t), string) => unit
    ) =>
    unit =
    "readFile";

  let read = (~encoding=Utf8, name) => {
    let encoding = string_of_encoding(encoding);
    readFile(~name, ~encoding)->Promise.fromCallback(parseError);
  };

  [@bs.scope "fs"] [@bs.val]
  external writeFile:
    (
      ~name: Path.t,
      ~content: string,
      ~encoding: string,
      Js.Nullable.t(RawJsError.t) => unit
    ) =>
    unit =
    "writeFile";

  let write = (~encoding=Utf8, ~name, content) => {
    let encoding = string_of_encoding(encoding);
    writeFile(~name, ~content, ~encoding)
    ->Promise.fromUnitCallback(parseError);
  };

  module CopyMode: {
    type t;
    let copy_ficlone: t;
  } = {
    type t = int;

    let copy_ficlone: t = [%raw "fs.constants.COPYFILE_FICLONE"];
  };

  [@bs.scope "fs"] [@bs.val]
  external copyFile:
    (
      ~name: Path.t,
      ~dest: Path.t,
      ~mode: CopyMode.t,
      Js.Nullable.t(RawJsError.t) => unit
    ) =>
    unit =
    "copyFile";

  let copy = (~name, ~dest, ~mode) => {
    copyFile(~name, ~dest, ~mode)->Promise.fromUnitCallback(parseError);
  };

  [@bs.scope "fs"] [@bs.val]
  external unlink:
    (~name: Path.t, Js.Nullable.t(RawJsError.t) => unit) => unit =
    "unlink";

  let rm = (~name) => {
    unlink(~name)->Promise.fromUnitCallback(parseError);
  };

  type rmdirOptions = {recursive: bool};

  [@bs.scope "fs"] [@bs.val]
  external rmdir:
    (
      ~path: Path.t,
      ~options: rmdirOptions,
      Js.Nullable.t(RawJsError.t) => unit
    ) =>
    unit =
    "rmdir";

  let rmdir = path => {
    rmdir(~path, ~options={recursive: true})
    ->Promise.fromUnitCallback(parseError);
  };

  type constant;

  type constants = {
    [@bs.as "W_OK"]
    wOk: constant,
  };

  [@bs.scope "fs"] [@bs.val] external constants: constants = "constants";

  [@bs.scope "fs"] [@bs.val]
  external access:
    (
      ~path: Path.t,
      ~constant: constant,
      Js.Nullable.t(RawJsError.t) => unit
    ) =>
    unit =
    "access";

  let access = path =>
    access(~path, ~constant=constants.wOk)
    ->Promise.fromUnitCallback(parseError)
    ->Promise.map(r => r->Result.isOk);

  let initIfNotExists = (~encoding=?, ~path, content) => {
    access(path)
    ->Promise.flatMap(access =>
        access ? Promise.value(Ok()) : write(~encoding?, ~name=path, content)
      );
  };

  [@bs.scope "fs"] [@bs.val]
  external mkdir: (Path.t, Js.Nullable.t(RawJsError.t) => unit) => unit =
    "mkdir";

  let mkdir = path => mkdir(path)->Promise.fromUnitCallback(parseError);

  let initDirIfNotExists = (path: Path.t) => {
    access(path)
    ->Promise.flatMap(access => access ? Promise.value(Ok()) : mkdir(path));
  };

  let mkTmpCopy = name => {
    copy(
      ~name,
      ~dest=!(Path.toString(name) ++ ".tmp"),
      ~mode=CopyMode.copy_ficlone,
    );
  };

  let restoreTmpCopy = name => {
    copy(
      ~name=!(Path.toString(name) ++ ".tmp"),
      ~dest=name,
      ~mode=CopyMode.copy_ficlone,
    );
  };

  let rmTmpCopy = name => {
    rm(~name=!(Path.toString(name) ++ ".tmp"));
  };

  let protect = (~name, ~transaction) =>
    name
    ->mkTmpCopy
    ->Promise.flatMapOk(_ => transaction()->Promise.map(v => Ok(v)))
    ->Promise.flatMapOk(r =>
        switch (r) {
        | Ok () => name->rmTmpCopy
        | Error(e) => name->restoreTmpCopy->Promise.map(_ => Error(e))
        }
      );
};

module Client = {
  open Path.Ops;

  let resetDir = path => File.rmdir(path);

  let initDir = baseDir => {
    File.initDirIfNotExists(baseDir)
    ->Promise.flatMapOk(() => {
        let secret =
          File.initIfNotExists(~path=baseDir / (!"secret_keys"), "[]");
        let public =
          File.initIfNotExists(~path=baseDir / (!"public_keys"), "[]");
        let pkh =
          File.initIfNotExists(~path=baseDir / (!"public_key_hashs"), "[]");
        Promise.mapOk3(secret, public, pkh, (_, _, _) => ());
      });
  };
};
