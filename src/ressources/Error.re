// Global errors description

type kind =
  | Operation
  | Connection
  | Account
  | Aliases
  | Balance
  | Delegate;

type timestamp = float;

type t = {
  kind,
  timestamp,
  msg: string,
};

let print_kind = e => {
  switch (e) {
  | Operation => "Operation"
  | Connection => "Connection"
  | Balance => "Balance"
  | Aliases => "Aliases"
  | Account => "Account"
  | Delegate => "Delegate"
  };
};
