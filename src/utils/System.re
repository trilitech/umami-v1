%raw
"
var Electron = window.require('electron');
var Process = window.require('process');
var OS = window.require('os');
var fs = window.require('fs');
var path = window.require('path'); ";

let electron = [%raw "Electron"];
let app = electron##remote##app;
let os = [%raw "OS"];

let getVersion = () => app##getVersion();

module Path: {
  type t = pri string;

  let mk: string => t;

  let toString: t => string;

  let join: array(t) => t;
  module Ops: {
    let (!): string => t;
    let (/): (t, t) => t;
  };
} = {
  type t = string;

  let mk = p => p;

  let toString = p => p;

  [@bs.module "path"] [@bs.variadic]
  external join: array(t) => string = "join";

  module Ops = {
    let (!) = a => mk(a);
    let (/) = (a, b) => join([|a, b|]);
  };
};

let appDir = () => Path.Ops.(!app##getPath("appData") / (!app##getName()));

let homeDir = () => os##homedir();

let getCurrentPath: unit => Path.t = () => app##getAppPath();

module File = {
  type error = {message: string};
  let mapError = e => e.message;

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
      (Js.Nullable.t(error), string) => unit
    ) =>
    unit =
    "readFile";

  let read = (~encoding=Utf8, name) => {
    let encoding = string_of_encoding(encoding);
    readFile(~name, ~encoding)->FutureEx.fromCallback(mapError);
  };

  [@bs.scope "fs"] [@bs.val]
  external writeFile:
    (
      ~name: Path.t,
      ~content: string,
      ~encoding: string,
      Js.Nullable.t(error) => unit
    ) =>
    unit =
    "writeFile";

  let write = (~encoding=Utf8, ~name, content) => {
    let encoding = string_of_encoding(encoding);
    writeFile(~name, ~content, ~encoding)
    ->FutureEx.fromUnitCallback(mapError);
  };

  type rmdirOptions = {recursive: bool};

  [@bs.scope "fs"] [@bs.val]
  external rmdir:
    (~path: Path.t, ~options: rmdirOptions, Js.Nullable.t(error) => unit) =>
    unit =
    "rmdir";

  let rmdir = path => {
    rmdir(~path, ~options={recursive: true})
    ->FutureEx.fromUnitCallback(mapError);
  };

  type constant;

  type constants = {
    [@bs.as "W_OK"]
    wOk: constant,
  };

  [@bs.scope "fs"] [@bs.val] external constants: constants = "constants";

  [@bs.scope "fs"] [@bs.val]
  external access:
    (~path: Path.t, ~constant: constant, Js.Nullable.t(error) => unit) => unit =
    "access";

  let access = path =>
    access(~path, ~constant=constants.wOk)
    ->FutureEx.fromUnitCallback(mapError)
    ->Future.map(r => r->Result.isOk);

  let initIfNotExists = (~encoding=?, ~path, content) => {
    access(path)
    ->Future.flatMap(access =>
        access ? Future.value(Ok()) : write(~encoding?, ~name=path, content)
      );
  };

  [@bs.scope "fs"] [@bs.val]
  external mkdir: (Path.t, Js.Nullable.t(error) => unit) => unit = "mkdir";

  let mkdir = path => mkdir(path)->FutureEx.fromUnitCallback(mapError);

  let initDirIfNotExists = (path: Path.t) => {
    access(path)
    ->Future.flatMap(access => access ? Future.value(Ok()) : mkdir(path));
  };
};

module Client = {
  open Path.Ops;

  let resetDir = path => File.rmdir(path);

  let initDir = baseDir => {
    File.initDirIfNotExists(baseDir)
    ->Future.flatMapOk(() => {
        let secret =
          File.initIfNotExists(~path=baseDir / (!"secret_keys"), "[]");
        let public =
          File.initIfNotExists(~path=baseDir / (!"public_keys"), "[]");
        let pkh =
          File.initIfNotExists(~path=baseDir / (!"public_key_hashs"), "[]");
        Future.mapOk3(secret, public, pkh, (_, _, _) => ());
      });
  };
};
