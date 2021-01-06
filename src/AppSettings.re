type network =
  | Mainnet
  | Testnet;

type sdks = {
  main: Js.Promise.t(TezosSDK.t),
  test: Js.Promise.t(TezosSDK.t),
};

type t = {
  config: ConfigFile.t,
  sdk: sdks,
  network,
};

let endpoint = settings =>
  switch (settings.network) {
  | Mainnet =>
    settings.config.endpointMain
    ->Belt.Option.getWithDefault(ConfigFile.endpointMain)
  | Testnet =>
    settings.config.endpointTest
    ->Belt.Option.getWithDefault(ConfigFile.endpointTest)
  };

let sdk = s =>
  switch (s.network) {
  | Mainnet => s.sdk.main
  | Testnet => s.sdk.test
  };

let testOnly = s => {...s, network: Testnet};
let mainOnly = s => {...s, network: Mainnet};
let withNetwork = (s, network) => {...s, network};

let explorer = (settings: t) =>
  switch (settings.network) {
  | Mainnet =>
    settings.config.explorerMain
    ->Belt.Option.getWithDefault(ConfigFile.explorerMain)

  | Testnet =>
    settings.config.explorerTest
    ->Belt.Option.getWithDefault(ConfigFile.explorerTest)
  };

let natviewer = settings =>
  switch (settings.network) {
  | Mainnet => assert(false)
  | Testnet =>
    settings.config.natviewerTest
    ->Belt.Option.getWithDefault(ConfigFile.natviewerTest)
  };
