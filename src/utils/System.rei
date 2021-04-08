let getVersion: unit => string;

module Path: {
  type t;

  let mk: string => t;

  let toString: t => string;

  let join: array(t) => t;

  module Ops: {
    let (!): string => t;

    let (/): (t, t) => t;
  };
};

let getCurrentPath: unit => Path.t;

let homeDir: unit => Path.t;
let appDir: unit => Path.t;

module File: {
  type encoding =
    | Utf8
    | Raw(string);

  let read:
    (~encoding: encoding=?, Path.t) => Future.t(Result.t(string, string));

  let write:
    (~encoding: encoding=?, ~name: Path.t, string) =>
    Future.t(Result.t(unit, string));

  let initIfNotExists:
    (~encoding: encoding=?, ~path: Path.t, string) =>
    Future.t(Result.t(unit, string));
  let initDirIfNotExists: Path.t => Future.t(Result.t(unit, string));
};

module Client: {
  let resetDir: Path.t => Future.t(Result.t(unit, string));

  let initDir: Path.t => Future.t(Result.t(unit, string));
};
