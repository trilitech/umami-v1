[@bs.module]
external format: (Js.Date.t, string) => string = "date-fns/format";

[@bs.module]
external differenceInDays: (Js.Date.t, Js.Date.t) => int =
  "date-fns/differenceInDays";

module Duration = {
  type t = {days: int};
};

[@bs.module]
external formatDuration: Duration.t => string = "date-fns/formatDuration";
