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

type t = ConfigFile.t;

let baseDir = (config: t) =>
  config.sdkBaseDir->Option.getWithDefault(ConfigFile.Default.sdkBaseDir);

let endpoint = (c: t) =>
  switch (c.network->Option.getWithDefault(ConfigFile.Default.network)) {
  | `Mainnet => Network.mainnet.endpoint
  | `Florencenet => Network.florencenet.endpoint
  | `Granadanet => Network.granadanet.endpoint
  | `Custom(name) =>
    c.customNetworks->List.getBy(n => n.name === name)->Option.getExn.endpoint
  };

let withNetwork = (c: t, network) => {...c, network};

let network = (config: t): ConfigFile.network =>
  config.network->Option.getWithDefault(ConfigFile.Default.network);

let chainId = (c: t) =>
  switch (network(c)) {
  | `Mainnet => Network.mainnet.chain
  | `Florencenet => Network.florencenet.chain
  | `Granadanet => Network.granadanet.chain
  | `Custom(name) =>
    c.customNetworks->List.getBy(n => n.name === name)->Option.getExn.chain
  };

let explorer = (c: t) =>
  switch (c.network->Option.getWithDefault(ConfigFile.Default.network)) {
  | `Mainnet => Network.mainnet.explorer
  | `Florencenet => Network.florencenet.explorer
  | `Granadanet => Network.granadanet.explorer
  | `Custom(name) =>
    c.customNetworks->List.getBy(n => n.name === name)->Option.getExn.explorer
  };

let externalExplorers =
  Map.String.empty
  ->Map.String.set(Network.mainnet.chain, "https://tzkt.io/")
  ->Map.String.set(Network.florencenet.chain, "https://florencenet.tzkt.io/");

let findExternalExplorer = c =>
  externalExplorers
  ->Map.String.get(c)
  ->Option.map(v => Ok(v))
  ->Option.getWithDefault(Error(`UnknownChainId(c)));

let getExternalExplorer = config => chainId(config)->findExternalExplorer;
