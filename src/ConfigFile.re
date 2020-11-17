type t = {
  endpointMain: option(string),
  endpointTest: option(string),
  explorerMain: option(string),
  explorerTest: option(string),
};

[@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";

let endpointMain = "https://mainnet-tezos.giganode.io";
let endpointTest = "https://delphinet-tezos.giganode.io";
let explorerMain = "https://mezos.lamini.ca/mezos/mainnet7";
let explorerTest = "https://mezos.lamini.ca/mezos/delphinet";

let default = {
  endpointMain: Some(endpointMain),
  endpointTest: Some(endpointTest),
  explorerMain: Some(explorerMain),
  explorerTest: Some(explorerTest),
};

let defaultToString = () =>
  default
  ->Js.Json.stringifyAny
  ->Belt.Option.map(Js.Json.parseExn)
  ->Belt.Option.map(j => Js.Json.stringifyWithSpace(j, 1));
