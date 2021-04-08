type sdks = {
  main: TezosSDK.t,
  test: TezosSDK.t,
};

type t = {
  config: ConfigFile.t,
  sdk: sdks,
};

let baseDir = settings =>
  settings.config.sdkBaseDir
  ->Option.getWithDefault(ConfigFile.Default.sdkBaseDir);

let endpoint = settings =>
  switch (settings.config.network) {
  | Some(`Mainnet) =>
    settings.config.endpointMain
    ->Option.getWithDefault(ConfigFile.Default.endpointMain)
  | None
  | Some(`Testnet) =>
    settings.config.endpointTest
    ->Option.getWithDefault(ConfigFile.Default.endpointTest)
  };

let sdk = s =>
  switch (s.config.network) {
  | Some(`Mainnet) => s.sdk.main
  | None
  | Some(`Testnet) => s.sdk.test
  };

let testOnly = s => {
  ...s,
  config: {
    ...s.config,
    network: Some(`Testnet),
  },
};
let mainOnly = s => {
  ...s,
  config: {
    ...s.config,
    network: Some(`Mainnet),
  },
};
let withNetwork = (s, network) => {
  ...s,
  config: {
    ...s.config,
    network,
  },
};

let network = (settings: t) =>
  settings.config.network->Option.getWithDefault(ConfigFile.Default.network);

let explorer = (settings: t) =>
  switch (settings.config.network) {
  | Some(`Mainnet) =>
    settings.config.explorerMain
    ->Option.getWithDefault(ConfigFile.Default.explorerMain)

  | None
  | Some(`Testnet) =>
    settings.config.explorerTest
    ->Option.getWithDefault(ConfigFile.Default.explorerTest)
  };
