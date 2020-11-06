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

let print_kind = ((), e) => {
  switch (e) {
  | Operation => "Operation"
  | Connection => "Connection"
  };
};

let print = ({kind, msg, timestamp}) => {
  Format.sprintf(
    "%s - %a : %s",
    Js.Date.(timestamp->fromFloat->toLocaleString),
    print_kind,
    kind,
    msg,
  );
};
