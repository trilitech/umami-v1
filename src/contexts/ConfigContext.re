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

open UmamiCommon;
type config = {
  content: ConfigFile.t,
  write: (ConfigFile.t => ConfigFile.t) => unit,
};

let initialState = {content: ConfigFile.dummy, write: _ => ()};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

let load = () => {
  switch (ConfigFile.read()->Js.Nullable.toOption) {
  | Some(conf) => ConfigFile.parse(conf)
  | None =>
    Js.log("No config to load. Using default config");
    ConfigFile.dummy;
  };
};

[@react.component]
let make = (~children) => {
  let (content, setConfig) = React.useState(() => load());

  let write = f =>
    setConfig(c => {
      let c = f(c);
      c->ConfigFile.toString->Lib.Option.iter(c => c->ConfigFile.write);
      c;
    });

  <Provider value={content, write}> children </Provider>;
};

let useContext = () => React.useContext(context);

let useWrite = () => {
  let store = useContext();
  store.write;
};

let useContent = () => {
  let store = useContext();
  store.content;
};

let useResetConfig = () => {
  let {write} = useContext();
  () => {
    write(_ => ConfigFile.dummy);
  };
};

let useCleanSdkBaseDir = () => {
  let {content: {sdkBaseDir}} = useContext();
  () => {
    System.Client.resetDir(
      sdkBaseDir->Option.getWithDefault(ConfigFile.Default.sdkBaseDir),
    )
    ->Future.tapOk(_ => LocalStorage.clear());
  };
};
