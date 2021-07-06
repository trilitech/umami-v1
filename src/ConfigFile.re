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

type network = [ | `Mainnet | `Testnet(string)];

type t = {
  network: option(network),
  endpointMain: option(string),
  endpointTest: option(string),
  explorerMain: option(string),
  explorerTest: option(string),
  theme: option([ | `system | `dark | `light]),
  confirmations: option(int),
  sdkBaseDir: option(System.Path.t),
};

[@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";

module Default = {
  /* let network = `Testnet(Network.edo2netChain); */
  let network = `Mainnet;
  let endpointMain = "https://mainnet.smartpy.io/";
  let endpointTest = "https://florencenet.smartpy.io/";
  let explorerMain = "https://api.umamiwallet.com/mainnet";
  let explorerTest = "https://api.umamiwallet.com/florencenet";
  let theme = `system;
  let sdkBaseDir = System.(Path.Ops.(appDir() / (!"tezos-client")));
  let confirmations = 5;
};

let dummy = {
  network: None,
  endpointMain: None,
  endpointTest: None,
  explorerMain: None,
  explorerTest: None,
  theme: None,
  confirmations: None,
  sdkBaseDir: None,
};

let toString = c =>
  c
  ->Js.Json.stringifyAny
  ->Option.map(Js.Json.parseExn)
  ->Option.map(j => Js.Json.stringifyWithSpace(j, 1));

let configKey = "Config";

let write = s => LocalStorage.setItem(configKey, s);

let read = () => LocalStorage.getItem(configKey);

let reset = () => LocalStorage.removeItem(configKey);

let getNetworkName = network =>
  switch (network) {
  | `Mainnet => Network.mainnetChain
  | `Testnet(c) => c->Network.getName
  };
