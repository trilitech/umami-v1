type t = {
  endpointMain: option(string),
  endpointTest: option(string),
  explorerMain: option(string),
  explorerTest: option(string),
  natviewerMain: option(string),
  natviewerTest: option(string),
  theme: option([ | `system | `dark | `light]),
  confirmations: option(int),
  sdkBaseDir: option(string),
};

[@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";

module Default = {
  let endpointMain = "https://mainnet-tezos.giganode.io";
  let endpointTest = "https://edonet-tezos.giganode.io";
  let explorerMain = "https://mezos.lamini.ca/mezos/mainnet7";
  let explorerTest = "https://mezos.lamini.ca/mezos/edonet";
  let natviewerTest = "KT1QW4QRMgmdKFvq6MZhk3x85eriyL5AVoP4";
  let theme = `system;
  let sdkBaseDir = System.homeDir() ++ "/.tezos-client";
  let confirmations = 5;
  let mkSdkEndpoint = url => url ++ ":443";
};

let dummy = {
  endpointMain: None,
  endpointTest: None,
  explorerMain: None,
  explorerTest: None,
  natviewerMain: None,
  natviewerTest: None,
  theme: None,
  confirmations: None,
  sdkBaseDir: None,
};

let toString = c =>
  c
  ->Js.Json.stringifyAny
  ->Option.map(Js.Json.parseExn)
  ->Option.map(j => Js.Json.stringifyWithSpace(j, 1));
