/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

%%raw("
var electron = require('electron');
var { app, Menu, MenuItem } = require('@electron/remote');
var shell = electron.shell;
var process = require('process');
var OS = require('os');
var fs = require('fs');
var path = require('path');

")

type Errors.t +=
  | NoSuchFileError(string)

@scope("app") @val external getVersion: unit => string = "getVersion"
@scope("app") @val external getName: unit => string = "getName"

@scope("shell") @val
external openExternal: string => Js.Promise.t<unit> = "openExternal"
let openExternal = url => url->openExternal->ignore

type plateform = [#darwin | #win32 | #linux]

@scope("process") @val external plateform: plateform = "platform"

let isMac = plateform == #darwin

@scope("process.env") @val
external nodeEnv: Js.Nullable.t<string> = "NODE_ENV"

@scope("process.env") @val
external appImage: Js.Nullable.t<string> = "APPIMAGE"

let isDev = nodeEnv->Js.Nullable.toOption->Option.mapWithDefault(false, a => a == "development")

@val external window: 'a = "window"

let reload = () => window["location"]["reload"]()

let hasAutoUpdate = () =>
  !isDev &&
  switch plateform {
  | #darwin => true
  | #win32 => true
  | #linux => !(appImage->Js.Nullable.isNullable)
  }

module Path: {
  type t = private string

  let mk: string => t

  let toString: t => string

  let join: array<t> => t

  let getCurrent: unit => t
  let getAppData: unit => t

  let baseName: (t, string) => string

  module Ops: {
    let not: string => t
    let \"/": (t, t) => t
  }
} = {
  type t = string

  let mk = p => p

  let toString = p => p

  @scope("app") @val external getCurrent: unit => t = "getAppPath"

  @scope("app") @val
  external getAppData: (@as("appData") _, unit) => t = "getPath"

  @module("path") @variadic
  external join: array<t> => string = "join"

  @module("path") @val
  external baseName: (t, string) => string = "basename"

  module Ops = {
    let not = a => mk(a)
    let \"/" = (a, b) => join([a, b])
  }
}

let appDir = () => {
  open Path.Ops
  Path.getAppData() / !getName()
}

module File = {
  open Path.Ops

  let no_such_file = "no such file"

  let parseError = (e: RawJsError.t) =>
    switch e.message {
    | s if s->Js.String2.includes(no_such_file) => NoSuchFileError(s)
    | s => Errors.Generic(s)
    }

  let () = Errors.registerHandler("System", x =>
    switch x {
    | NoSuchFileError(s) => s->Some
    | _ => None
    }
  )

  type encoding =
    | Utf8
    | Raw(string)

  let string_of_encoding = e =>
    switch e {
    | Utf8 => "utf8"
    | Raw(s) => s
    }

  @scope("fs") @val
  external readFile: (
    ~name: Path.t,
    ~encoding: string,
    (Js.Nullable.t<RawJsError.t>, string) => unit,
  ) => unit = "readFile"

  let read = (~encoding=Utf8, name) => {
    let encoding = string_of_encoding(encoding)
    readFile(~name, ~encoding)->Promise.fromCallback(parseError)
  }

  @scope("fs") @val
  external writeFile: (
    ~name: Path.t,
    ~content: string,
    ~encoding: string,
    Js.Nullable.t<RawJsError.t> => unit,
  ) => unit = "writeFile"

  let write = (~encoding=Utf8, ~name, content) => {
    let encoding = string_of_encoding(encoding)
    writeFile(~name, ~content, ~encoding)->Promise.fromUnitCallback(parseError)
  }

  module CopyMode: {
    type t
    let copy_ficlone: t
  } = {
    type t = int

    let copy_ficlone: t = %raw("fs.constants.COPYFILE_FICLONE")
  }

  @scope("fs") @val
  external copyFile: (
    ~name: Path.t,
    ~dest: Path.t,
    ~mode: CopyMode.t,
    Js.Nullable.t<RawJsError.t> => unit,
  ) => unit = "copyFile"

  let copy = (~name, ~dest, ~mode) =>
    copyFile(~name, ~dest, ~mode)->Promise.fromUnitCallback(parseError)

  @scope("fs") @val
  external unlink: (~name: Path.t, Js.Nullable.t<RawJsError.t> => unit) => unit = "unlink"

  let rm = (~name) => unlink(~name)->Promise.fromUnitCallback(parseError)

  type rmdirOptions = {recursive: bool}

  @scope("fs") @val
  external rmdir: (
    ~path: Path.t,
    ~options: rmdirOptions,
    Js.Nullable.t<RawJsError.t> => unit,
  ) => unit = "rmdir"

  let rmdir = path => rmdir(~path, ~options={recursive: true})->Promise.fromUnitCallback(parseError)

  type constant

  type constants = {
    @as("W_OK")
    wOk: constant,
  }

  @scope("fs") @val external constants: constants = "constants"

  @scope("fs") @val
  external access: (
    ~path: Path.t,
    ~constant: constant,
    Js.Nullable.t<RawJsError.t> => unit,
  ) => unit = "access"

  let access = path =>
    access(~path, ~constant=constants.wOk)
    ->Promise.fromUnitCallback(parseError)
    ->Promise.map(r => r->Result.isOk)

  let initIfNotExists = (~encoding=?, ~path, content) =>
    access(path)->Promise.flatMap(access =>
      access ? Promise.value(Ok()) : write(~encoding?, ~name=path, content)
    )

  @scope("fs") @val
  external mkdir: (Path.t, Js.Nullable.t<RawJsError.t> => unit) => unit = "mkdir"

  let mkdir = path => mkdir(path)->Promise.fromUnitCallback(parseError)

  let initDirIfNotExists = (path: Path.t) =>
    access(path)->Promise.flatMap(access => access ? Promise.value(Ok()) : mkdir(path))

  let mkTmpCopy = name =>
    copy(~name, ~dest=!(Path.toString(name) ++ ".tmp"), ~mode=CopyMode.copy_ficlone)

  let restoreTmpCopy = name =>
    copy(~name=!(Path.toString(name) ++ ".tmp"), ~dest=name, ~mode=CopyMode.copy_ficlone)

  let rmTmpCopy = name => rm(~name=!(Path.toString(name) ++ ".tmp"))

  let protect = (~name, ~transaction) =>
    name
    ->mkTmpCopy
    ->Promise.flatMapOk(_ => transaction()->Promise.map(v => Ok(v)))
    ->Promise.flatMapOk(r =>
      switch r {
      | Ok() => name->rmTmpCopy
      | Error(e) => name->restoreTmpCopy->Promise.map(_ => Error(e))
      }
    )
}

module Client = {
  open Path.Ops

  let resetDir = path => File.rmdir(path)

  let initDir = baseDir =>
    File.initDirIfNotExists(baseDir)->Promise.flatMapOk(() => {
      let secret = File.initIfNotExists(~path=baseDir / !"secret_keys", "[]")
      let public = File.initIfNotExists(~path=baseDir / !"public_keys", "[]")
      let pkh = File.initIfNotExists(~path=baseDir / !"public_key_hashs", "[]")
      Promise.mapOk3(secret, public, pkh, (_, _, _) => ())
    })
}
