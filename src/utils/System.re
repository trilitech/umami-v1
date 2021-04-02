%raw
"
var Electron = window.require('electron');
var Process = window.require('process');
var OS = window.require('os');
var fs = window.require('fs'); ";

let electron = [%raw "Electron"];
let app = electron##remote##app;
let os = [%raw "OS"];

let homeDir = () => os##homedir();

let getCurrentPath: unit => string = () => app##getAppPath();

let getVersion = () => app##getVersion();

let appDir = () => app##getPath("appData") ++ "/" ++ app##getName();

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
      ~name: string,
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
      ~name: string,
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
    (~path: string, ~options: rmdirOptions, Js.Nullable.t(error) => unit) =>
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
    (~path: string, ~constant: constant, Js.Nullable.t(error) => unit) => unit =
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
  external mkdir: (string, Js.Nullable.t(error) => unit) => unit = "mkdir";

  let mkdir = path => mkdir(path)->FutureEx.fromUnitCallback(mapError);

  let initDirIfNotExists = path => {
    access(path)
    ->Future.flatMap(access => access ? Future.value(Ok()) : mkdir(path));
  };
};

module Client = {
  let resetDir = path => File.rmdir(path);

  let initDir = baseDir => {
    File.initDirIfNotExists(baseDir)
    ->Future.flatMapOk(() => {
        let secret =
          File.initIfNotExists(~path=baseDir ++ "/secret_keys", "[]");
        let public =
          File.initIfNotExists(~path=baseDir ++ "/public_keys", "[]");
        let pkh =
          File.initIfNotExists(~path=baseDir ++ "/public_key_hashs", "[]");
        Future.mapOk3(secret, public, pkh, (_, _, _) => ());
      });
  };
};
