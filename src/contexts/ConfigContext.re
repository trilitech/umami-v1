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

type configState = {
  content: env,
  configFile: ConfigFile.t,
  write: (ConfigFile.t => ConfigFile.t) => unit,
  networkStatus,
  retryNetwork:
    (~onlyOn: Network.status=?, ~onlyAfter: Network.status=?, unit) => unit,
  storageVersion: Version.t,
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
  storageVersion: Version.mk(1, 0),
};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

let initMigration = () => {
  let version =
    switch (LocalStorage.Version.get()) {
    | Ok(v) => v
    | Error(_) =>
      Js.log("Storage version not found, using 1.0 as base");
      Version.mk(1, 0);
    };

  switch (Migration.init(version)) {
  | Ok () => LocalStorage.Version.set(Migration.currentVersion)
  | Error(_) => ()
  };
};

let load = () => {
  initMigration();

  switch (ConfigFile.read()) {
  | Ok(conf) => conf
  | Error(_) =>
    Js.log("No config to load. Using default config");
    ConfigFile.dummy;
  };
};

let version = () => {
  switch (LocalStorage.Version.get()) {
  | Ok(v) => v
  | Error(_) =>
    Js.log("Storage version not found, using 1.0 as base");
    Version.mk(1, 0);
  };
};

[@react.component]
let make = (~children) => {
  let (configFile, setConfig) = React.useState(() => load());
  let (storageVersion, _) = React.useState(() => version());

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

  let (lastCheck, setLastCheck) = React.useState(() => None);

  let checkNetwork = timeout => {
    FutureEx.async(() => {
      FutureEx.timeout(timeout)
      ->Future.mapOk(() => {
          setLastCheck(
            fun
            | None => None
            | Some(n) => Some(n + 1),
          );
          pickNetwork();
        })
    });
  };

  React.useEffect1(
    () => {
      switch (lastCheck) {
      | Some(0) => checkNetwork(15000)
      | Some(1) => checkNetwork(30000)
      | Some(_n) => checkNetwork(60000)
      | None => ()
      };

      None;
    },
    [|lastCheck|],
  );

  React.useEffect1(
    () => {
      if (networkStatus.current == Offline
          && (
            networkStatus.previous == Some(Online)
            || networkStatus.previous == None
          )) {
        setLastCheck(_ => Some(0));
      } else if (networkStatus.current == Online
                 && networkStatus.previous == Some(Offline)) {
        setLastCheck(_ => None);
      } else {
        ();
      };

      None;
    },
    [|networkStatus|],
  );

  let write = f =>
    setConfig(c => {
      let c = f(c);
      c->ConfigFile.Storage.set;
      c;
    });

  <Provider
    value={
      content,
      configFile,
      write,
      networkStatus,
      retryNetwork,
      storageVersion,
    }>
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
