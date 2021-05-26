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

let endpoint = s =>
  switch (s.config.network->Option.getWithDefault(ConfigFile.Default.network)) {
  | `Mainnet => endpointMain(s)
  | `Testnet(_) => endpointTest(s)
  };

let sdk = s =>
  switch (s.config.network->Option.getWithDefault(ConfigFile.Default.network)) {
  | `Mainnet => s.sdk.main
  | `Testnet(_) => s.sdk.test
  };

let testOnly = (s, chainId) => {
  ...s,
  config: {
    ...s.config,
    network: Some(`Testnet(chainId)),
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

let chainId = (settings: t) =>
  switch (network(settings)) {
  | `Mainnet => Network.mainnetChain
  | `Testnet(chainId) => chainId
  };

let explorerMain = settings =>
  settings.config.explorerMain
  ->Option.getWithDefault(ConfigFile.Default.explorerMain);

let explorerTest = settings =>
  settings.config.explorerTest
  ->Option.getWithDefault(ConfigFile.Default.explorerTest);

let explorer = (s: t) =>
  switch (s.config.network->Option.getWithDefault(ConfigFile.Default.network)) {
  | `Mainnet => explorerMain(s)
  | `Testnet(_) => explorerTest(s)
  };

let externalExplorers =
  Map.String.empty
  ->Map.String.set(Network.mainnetChain, "https://tzkt.io/")
  ->Map.String.set(Network.florencenetChain, "https://florencenet.tzkt.io/")
  ->Map.String.set(Network.edo2netChain, "https://edo2net.tzkt.io/");

let findExternalExplorer = c =>
  externalExplorers
  ->Map.String.get(c)
  ->Option.map(v => Ok(v))
  ->Option.getWithDefault(Error(Network.UnknownChainId(c)));

let getExternalExplorer = settings => chainId(settings)->findExternalExplorer;
