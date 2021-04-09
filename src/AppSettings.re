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

let endpointMain = settings =>
  settings.config.endpointMain
  ->Option.getWithDefault(ConfigFile.Default.endpointMain);

let endpointTest = settings =>
  settings.config.endpointTest
  ->Option.getWithDefault(ConfigFile.Default.endpointTest);

let endpoint = settings =>
  switch (settings.config.network) {
  | Some(`Mainnet) => endpointMain(settings)
  | None
  | Some(`Testnet) => endpointTest(settings)
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

let network = (settings: t): ConfigFile.network =>
  settings.config.network->Option.getWithDefault(ConfigFile.Default.network);

let explorerMain = settings =>
  settings.config.explorerMain
  ->Option.getWithDefault(ConfigFile.Default.explorerMain);

let explorerTest = settings =>
  settings.config.explorerTest
  ->Option.getWithDefault(ConfigFile.Default.explorerTest);

let explorer = (settings: t) =>
  switch (settings.config.network) {
  | Some(`Mainnet) => explorerMain(settings)
  | None
  | Some(`Testnet) => explorerTest(settings)
  };
