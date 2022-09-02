/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2022 Nomadic Labs, <contact@nomadic-labs.com>          */
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

type notice_kind =
  | Notice_network_unreachable
  | Notice_update_downloaded
  | Notice_update_required;

type state = {
  notices: list(notice_kind), /* Meant to be used as LIFO */
  push: notice_kind => unit,
  delete: notice_kind => unit,
};

let initialState = {notices: [], push: _ => (), delete: _ => ()};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

[@react.component]
let make = (~children) => {
  let (notices, setNotices) = React.useState(() => []);

  let delete = (key: notice_kind) => {
    setNotices(es => es->List.keep(n => n != key));
  };

  let push = n => {
    setNotices(es => {[n, ...es]});
  };

  let apiVersion = StoreContext.useApiVersion();
  React.useEffect0(_ => {
    let displayUpdateNotice =
      apiVersion
      ->Option.map(apiVersion =>
          !Network.checkInBound(apiVersion.Network.api)
        )
      ->Option.getWithDefault(false);
    if (displayUpdateNotice) {
      push(Notice_update_required);
    };
    None;
  });

  let networkOffline = ConfigContext.useNetworkOffline();
  React.useEffect1(
    _ => {
      (networkOffline ? push : delete)(Notice_network_unreachable);
      None;
    },
    [|networkOffline|],
  );

  React.useEffect0(_ => {
    IPC.on("update-downloaded", (_, _) => push(Notice_update_downloaded));
    None;
  });

  <Provider value={notices, push, delete}> children </Provider>;
};

let useStoreContext = () => React.useContext(context);

let useNotices = () => {
  let store = useStoreContext();
  store.notices;
};

let usePush = () => {
  let store = useStoreContext();
  store.push;
};

let useDelete = () => {
  let store = useStoreContext();
  store.delete;
};
