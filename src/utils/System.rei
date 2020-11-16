let getCurrentPath: unit => string;

module File: {
  type encoding =
    | Utf8
    | Raw(string);

  let read:
    (~encoding: encoding=?, string) =>
    Future.t(Belt.Result.t(string, string));

  let write:
    (~encoding: encoding=?, ~name: string, string) =>
    Future.t(Belt.Result.t(unit, string));
};

module Config: {
  let write: string => Future.t(Belt.Result.t(unit, string));
  let read: unit => Future.t(Belt.Result.t(string, string));
};
