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

type env = {
  network: Network.network,
  defaultNetwork: bool,
  theme: [ | `system | `dark | `light],
  confirmations: int,
  baseDir: unit => System.Path.t,
};

let defaultNetwork = `Mainnet;

let default = {
  network: Network.mainnet,
  defaultNetwork: true,
  theme: `system,
  baseDir: () => System.(Path.Ops.(appDir() / (!"tezos-client"))),
  confirmations: 5,
};

let fromFile = f => {
  theme: f.theme->Option.getWithDefault(`system),
  defaultNetwork:
    switch (f.ConfigFile.network) {
    | Some(`Custom(_)) => false
    | _ => true
    },
  confirmations:
    f.confirmations->Option.getWithDefault(default.confirmations),
  baseDir:
    f.sdkBaseDir->Option.mapWithDefault(default.baseDir, (bd, ()) => bd),
  network:
    switch (f.ConfigFile.network) {
    | None
    | Some(`Mainnet) => Network.mainnet
    | Some(`Granadanet) => Network.granadanet
    | Some(`Custom(name)) =>
      f.customNetworks
      ->List.getBy(n => n.name === name)
      ->Option.getWithDefault(Network.mainnet)
    },
};

type networkStatus = {
  previous: option(Network.status),
  current: Network.status,
};

open UmamiCommon;
type configState = {
  content: env,
  configFile: ConfigFile.t,
  write: (ConfigFile.t => ConfigFile.t) => unit,
  networkStatus,
  retryNetwork:
    (~onlyOn: Network.status=?, ~onlyAfter: Network.status=?, unit) => unit,
};

let initialState = {
  configFile: ConfigFile.dummy,
  content: default,
  write: _ => (),
  networkStatus: {
    previous: None,
    current: Pending,
  },
  retryNetwork: (~onlyOn as _=?, ~onlyAfter as _=?, ()) => (),
};

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
  let (configFile, setConfig) = React.useState(() => load());

  let (content, setContent) = React.useState(() => load()->fromFile);

  let (networkStatus, setNetworkStatus) =
    React.useState(() => {previous: None, current: Network.Pending});

  let updateNetwork = n =>
    setNetworkStatus(prev => {
      let previous =
        prev.current == Pending ? prev.previous : Some(prev.current);

      {current: n, previous};
    });

  let pickNetwork = () => {
    updateNetwork(Network.Pending);
    let config = configFile->fromFile;

    let network =
      switch (configFile.network->Option.getWithDefault(defaultNetwork)) {
      | `Custom(_) =>
        Network.testNetwork(config.network)->Future.mapOk(_ => config.network)
      | #Network.nativeChains as net => Network.findValidEndpoint(net)
      };

    network->Future.get(
      fun
      | Ok(network) => {
          updateNetwork(Online);
          setContent(_ => {...config, network});
        }
      | Error(_) => updateNetwork(Offline),
    );
  };

  let retryNetwork = (~onlyOn=?, ~onlyAfter=?, ()) => {
    switch (onlyOn, onlyAfter) {
    | (None, None) => pickNetwork()
    | (Some(on), Some(_) as after) =>
      after == networkStatus.previous && on == networkStatus.current
        ? pickNetwork() : ()
    | (Some(on), None) => on == networkStatus.current ? pickNetwork() : ()
    | (None, Some(_) as after) =>
      after == networkStatus.previous ? pickNetwork() : ()
    };
  };

  React.useEffect1(
    () => {
      pickNetwork();
      None;
    },
    [|configFile|],
  );

  let write = f =>
    setConfig(c => {
      let c = f(c);
      c->ConfigFile.toString->Lib.Option.iter(c => c->ConfigFile.write);
      c;
    });

  <Provider value={content, configFile, write, networkStatus, retryNetwork}>
    children
  </Provider>;
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

let useFile = () => {
  let store = useContext();
  store.configFile;
};

let useResetConfig = () => {
  let {write} = useContext();
  () => {
    write(_ => ConfigFile.dummy);
  };
};

let useNetworkStatus = () => useContext().networkStatus;
let useNetworkOffline = () => useContext().networkStatus.current == Offline;

let useRetryNetwork = () => useContext().retryNetwork;

let useCleanSdkBaseDir = () => {
  let {content: {baseDir}} = useContext();
  () => {
    System.Client.resetDir(baseDir())
    ->Future.tapOk(_ => LocalStorage.clear());
  };
};
