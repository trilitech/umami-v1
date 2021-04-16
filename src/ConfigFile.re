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
  let network = `Testnet(Network.edo2netChain);
  let endpointMain = "https://mainnet.smartpy.io/";
  let endpointTest = "https://edonet.smartpy.io/";
  let explorerMain = "https://api.umamiwallet.com/mainnet";
  let explorerTest = "https://api.umamiwallet.com/edo2net";
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
