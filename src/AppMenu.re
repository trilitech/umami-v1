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
const { shell, Menu, MenuItem } = require('electron')
";

[@bs.scope "shell"] [@bs.val]
external openExternal: string => Js.Promise.t(unit) = "openExternal";
let openExternal = url => url->openExternal->ignore;

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

module Item = {
  type t = {role: option(role)};
  type options = {
    role: option(role),
    [@bs.as "type"]
    kind: option(kind),
    label: option(string),
    click: option(ReactNative.Event.pressEvent => unit),
    submenu: option(array(options)),
  };

  [@bs.new] external make: options => t = "MenuItem";
};

let makeItem = (~role=?, ~kind=?, ~label=?, ~click=?, ~submenu=?, ()) => {
  Item.role,
  kind,
  click,
  label,
  submenu,
};

let makeSubmenu = (~role=?, ~label=?, ~submenu, ()) => {
  Item.role,
  kind: Some(`submenu),
  click: None,
  label,
  submenu: Some(submenu),
};

type t = {items: array(Item.t)};

[@bs.new] external make: unit => t = "Menu";

[@bs.scope "Menu"] [@bs.val]
external getApplicationMenuRaw: unit => Js.Nullable.t(t) =
  "getApplicationMenu";
let getApplicationMenu: unit => option(t) =
  () => getApplicationMenuRaw()->Js.Nullable.toOption;

[@bs.scope "Menu"] [@bs.val]
external setApplicationMenu: t => unit = "setApplicationMenu";

[@bs.send] external append: (t, Item.t) => unit = "append";

let setAppMenu = () => {
  let supportUrl = "https://umamiwallet.com/#support";
  let downloadUrl = "https://umamiwallet.com/#download";
  let websiteUrl = "https://umamiwallet.com";

  let supportItem =
    makeItem(
      ~label=I18n.Menu.app_menu_support,
      ~click=_ => openExternal(supportUrl),
      (),
    );
  let downloadItem =
    makeItem(
      ~label=I18n.Menu.app_menu_new_version,
      ~click=_ => openExternal(downloadUrl),
      (),
    );
  let websiteItem =
    makeItem(
      ~label=I18n.Menu.app_menu_website,
      ~click=_ => openExternal(websiteUrl),
      (),
    );

  let currentAppMenu = getApplicationMenu();

  let newAppMenu = make();
  currentAppMenu
  ->Option.map(menu =>
      menu.items->Js.Array2.filter(item => item.role != Some(`help))
    )
  ->Option.iter(items =>
      items->Array.forEach(item => newAppMenu->append(item))
    );
  newAppMenu->append(
    Item.make(
      makeSubmenu(
        ~role=`help,
        ~label=I18n.Menu.app_menu_help,
        ~submenu=[|supportItem, downloadItem, websiteItem|],
        (),
      ),
    ),
  );

  setApplicationMenu(newAppMenu);
};
