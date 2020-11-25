// Global errors description

type origin =
  | Operation
  | Connection
  | Account
  | Aliases
  | Balance
  | Delegate;

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

let originToString = e => {
  switch (e) {
  | Operation => "Operation"
  | Connection => "Connection"
  | Balance => "Balance"
  | Aliases => "Aliases"
  | Account => "Account"
  | Delegate => "Delegate"
  };
};
