%raw
"
var Electron = window.require('electron');
var Process = window.require('process');
var OS = window.require('os');
var Fs = window.require('fs'); ";

let electron = [%raw "Electron"];
let app = electron##remote##app;
let fs = [%raw "Fs"];
let os = [%raw "OS"];

let homeDir = () => os##homedir();

let getCurrentPath: unit => string = () => app##getAppPath();

module File = {
  type encoding =
    | Utf8
    | Raw(string);

  let string_of_encoding = e =>
    switch (e) {
    | Utf8 => "utf8"
    | Raw(s) => s
    };

  let read =
      (~encoding: encoding=Utf8, name: string)
      : Future.t(Result.t(string, string)) => {
    let encoding_str = string_of_encoding(encoding);
    Future.make(resolve => {
      fs##readFile(name, encoding_str, (e, data) => {
        switch (Js.Nullable.toOption(e)) {
        | Some(e) => Error(e##message)->resolve
        | None => Ok(data)->resolve
        }
      })
    });
  };

  let write =
      (~encoding: encoding=Utf8, ~name: string, content: string)
      : Future.t(Result.t(unit, string)) => {
    let encoding_str = string_of_encoding(encoding);
    Future.make(resolve => {
      fs##writeFile(name, content, encoding_str, e => {
        switch (Js.Nullable.toOption(e)) {
        | Some(e) => Error(e)->resolve
        | None => Ok()->resolve
        }
      })
    });
  };

  type rmdirOption = {recursive: bool};

  let rmdir = (path: string): Future.t(Result.t(unit, string)) => {
    Future.make(resolve => {
      fs##rmdir(path, {recursive: true}, e => {
        switch (Js.Nullable.toOption(e)) {
        | Some(e) => Error(e)->resolve
        | None => Ok()->resolve
        }
      })
    });
  };
};

module Config = {
  let getPath: unit => string =
    () => app##getPath("appData") ++ "/" ++ app##getName() ++ "/Config";

  let write = s => File.write(~name=getPath(), s);
  let read = () => File.read(getPath());
};

module Client = {
  let reset = path => File.rmdir(path);
};
