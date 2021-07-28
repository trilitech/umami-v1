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

type config = {
  config: ConfigFile.t,
  sdkMain: TezosSDK.t,
  sdkTest: TezosSDK.t,
};

let initialState = {
  config: ConfigFile.dummy,
  sdkMain: TezosSDK.dummySdk,
  sdkTest: TezosSDK.dummySdk,
};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

[@react.component]
let make = (~empty, ~children) => {
  let config = ConfigContext.useContent();

  let (sdkMain, setSdkMain) = React.useState(() => TezosSDK.dummySdk);
  let (sdkTest, setSdkTest) = React.useState(() => TezosSDK.dummySdk);
  let (loaded, setLoaded) = React.useState(() => false);

  let loadSdk = (conf: ConfigFile.t) => {
    let dir =
      ConfigFile.(conf.sdkBaseDir->Option.getWithDefault(Default.sdkBaseDir));

    let pMain =
      TezosSDK.init(System.Path.toString(dir), Network.mainnet.endpoint)
      |> Js.Promise.then_(sdk => {
           setSdkMain(_ => sdk);
           Js.Promise.resolve();
         });

    let pTest =
      TezosSDK.init(System.Path.toString(dir), Network.florencenet.endpoint)
      |> Js.Promise.then_(sdk => {
           setSdkTest(_ => sdk);
           Js.Promise.resolve();
         });

    Js.Promise.all([|pMain, pTest|])
    |> Js.Promise.then_(_ => setLoaded(_ => true)->Js.Promise.resolve)
    |> ignore;
  };

  let load = () => {
    loadSdk(config);
  };

  React.useEffect0(() => {
    load();
    None;
  });

  <Provider value={config, sdkMain, sdkTest}>
    {loaded ? children : empty()}
  </Provider>;
};

let useContext = () => React.useContext(context);

let useSettings = () => {
  let store = useContext();
  AppSettings.{
    config: store.config,
    sdk: {
      main: store.sdkMain,

      test: store.sdkTest,
    },
  };
};
