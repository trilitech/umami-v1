// Global errors description

type kind =
  | Operation
  | Connection;

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
  };
};
