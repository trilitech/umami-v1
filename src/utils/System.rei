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

module Menu: {
  type kind = [ | `normal | `separator | `submenu | `checkbox | `radio];

  type role = [
    | `undo
    | `about
    | `redo
    | `cut
    | `copy
    | `paste
    | `pasteAndMatchStyle
    | `selectAll
    | `delete
    | `minimize
    | `close
    | `quit
    | `reload
    | `forceReload
    | `toggleDevTools
    | `togglefullscreen
    | `resetZoom
    | `zoomIn
    | `zoomOut
    | `toggleSpellChecker
    | `fileMenu
    | `editMenu
    | `viewMenu
    | `windowMenu
    | `appMenu
    | `hide
    | `hideOthers
    | `unhide
    | `startSpeaking
    | `stopSpeaking
    | `front
    | `zoom
    | `toggleTabBar
    | `selectNextTab
    | `selectPreviousTab
    | `mergeAllWindows
    | `moveTabToNewWindow
    | `window
    | `help
    | `services
    | `recentDocuments
    | `clearRecentDocuments
    | `shareMenu
  ];

  type item = {
    role: option(role),
    [@bs.as "type"]
    kind: option(kind),
    label: option(string),
    click: option(ReactNative.Event.pressEvent => unit),
    submenu: option(array(item)),
  };

  let mkItem:
    (
      ~role: role=?,
      ~kind: kind=?,
      ~label: string=?,
      ~click: ReactNative.Event.pressEvent => unit=?,
      ~submenu: array(item)=?,
      unit
    ) =>
    item;

  let mkSubmenu:
    (~role: role=?, ~label: string=?, ~submenu: array(item), unit) => item;

  type template = array(item);
  type menu;

  let buildFromTemplate: template => menu;
  let setApplicationMenu: menu => unit;
};
