type t = {
  endpointMain: option(string),
  endpointTest: option(string),
  explorerMain: option(string),
  explorerTest: option(string),
  natviewerMain: option(string),
  natviewerTest: option(string),
  theme: option([ | `dark | `light]),
  sdkBaseDir: option(string),
};

[@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";

let endpointMain = "https://mainnet-tezos.giganode.io";
let endpointTest = "https://edonet-tezos.giganode.io";
let explorerMain = "https://mezos.lamini.ca/mezos/mainnet7";
let explorerTest = "https://mezos.lamini.ca/mezos/delphinet";
let natviewerTest = "KT1QW4QRMgmdKFvq6MZhk3x85eriyL5AVoP4";
let theme = `dark;
let sdkBaseDir = System.homeDir() ++ "/.tezos-client";

let mkSdkEndpoint = url => url ++ ":443";

let default = {
  endpointMain: Some(endpointMain),
  endpointTest: Some(endpointTest),
  explorerMain: Some(explorerMain),
  explorerTest: Some(explorerTest),
  natviewerMain: None,
  natviewerTest: Some(natviewerTest),
  theme: Some(theme),
  sdkBaseDir: Some(sdkBaseDir),
};

let toString = c =>
  c
  ->Js.Json.stringifyAny
  ->Option.map(Js.Json.parseExn)
  ->Option.map(j => Js.Json.stringifyWithSpace(j, 1));

let defaultToString = () => default->toString;
