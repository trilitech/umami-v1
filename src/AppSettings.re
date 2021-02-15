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
    ->Option.getWithDefault(ConfigFile.Default.endpointMain)
  | Testnet =>
    settings.config.endpointTest
    ->Option.getWithDefault(ConfigFile.Default.endpointTest)
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
    ->Option.getWithDefault(ConfigFile.Default.explorerMain)

  | Testnet =>
    settings.config.explorerTest
    ->Option.getWithDefault(ConfigFile.Default.explorerTest)
  };
