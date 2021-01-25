// Global errors description

type origin =
  | Operation
  | Connection
  | Account
  | Aliases
  | Global
  | Delegate
  | Balance
  | Tokens;

type kind =
  | Info
  | Error;

type timestamp = float;

type t = {
  kind,
  origin,
  timestamp,
  msg: string,
};

let log = (~kind, ~origin=Global, msg) => {
  kind,
  timestamp: Js.Date.now(),
  origin,
  msg,
};

let info = log(~kind=Info);
let error = log(~kind=Error);

let originToString = e => {
  switch (e) {
  | Global => "Global"
  | Operation => "Operation"
  | Connection => "Connection"
  | Balance => "Balance"
  | Aliases => "Aliases"
  | Account => "Account"
  | Delegate => "Delegate"
  | Tokens => "Tokens"
  };
};
