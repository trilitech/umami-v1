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

  let read:
    (~encoding: encoding=?, Path.t) => Future.t(Result.t(string, Errors.t));

  let write:
    (~encoding: encoding=?, ~name: Path.t, string) =>
    Future.t(Result.t(unit, Errors.t));

  module CopyMode: {
    type t;
    let copy_excl: t;
    let copy_ficlone: t;
    let copy_ficlone_force: t;

    let assemble: (t, t) => t;
  };

  let access: Path.t => Future.t(bool);

  let copy:
    (~name: Path.t, ~dest: Path.t, ~mode: CopyMode.t) =>
    Future.t(result(unit, Errors.t));

  let rm: (~name: Path.t) => Future.t(result(unit, Errors.t));

  let initIfNotExists:
    (~encoding: encoding=?, ~path: Path.t, string) =>
    Future.t(Result.t(unit, Errors.t));
  let initDirIfNotExists: Path.t => Future.t(Result.t(unit, Errors.t));
};

module Client: {
  let resetDir: Path.t => Future.t(Result.t(unit, Errors.t));

  let initDir: Path.t => Future.t(Result.t(unit, Errors.t));
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
