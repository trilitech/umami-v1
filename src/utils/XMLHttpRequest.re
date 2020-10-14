type t;

module Response = {
  type t;

  [@bs.get] external json: t => Js.Json.t = "json";
};

[@bs.new] external make: unit => t = "XMLHttpRequest";
[@bs.send]
external addEventListener: (t, string, unit => unit) => unit =
  "addEventListener";
[@bs.get] external response: t => Response.t = "response";
[@bs.send] external open_: (t, string, string) => unit = "open";
[@bs.send] external send: t => unit = "send";
[@bs.send] external abort: t => unit = "abort";

[@bs.get] external status: t => int = "status";