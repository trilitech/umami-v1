type t = {
  endpointMain: option(string),
  endpointTest: option(string),
  explorerMain: option(string),
  explorerTest: option(string),
  theme: option([ | `system | `dark | `light]),
  confirmations: option(int),
  sdkBaseDir: option(string),
};

[@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";

module Default = {
  let endpointMain = "https://mainnet-tezos.giganode.io";
  let endpointTest = "https://api.umamiwallet.com/node/edo2net";
  let explorerMain = "https://mezos.lamini.ca/mezos/mainnet7";
  let explorerTest = "https://api.umamiwallet.com/edo2net";
  let theme = `system;
  let sdkBaseDir = System.homeDir() ++ "/.tezos-client";
  let confirmations = 5;
};

let dummy = {
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
