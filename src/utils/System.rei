let getCurrentPath: unit => string;

let homeDir: unit => string;

let getVersion: unit => string;

module File: {
  type encoding =
    | Utf8
    | Raw(string);

  let read:
    (~encoding: encoding=?, string) => Future.t(Result.t(string, string));

  let write:
    (~encoding: encoding=?, ~name: string, string) =>
    Future.t(Result.t(unit, string));
};

module Client: {let reset: string => Future.t(Result.t(unit, string));};
