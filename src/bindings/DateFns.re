[@bs.module "date-fns/format"]
external format: (Js.Date.t, string) => string = "default";

[@bs.module "date-fns/differenceInDays"]
external differenceInDays: (Js.Date.t, Js.Date.t) => int = "default";

module Duration = {
  type t = {days: int};
};

[@bs.module "date-fns/formatDuration"]
external formatDuration: Duration.t => string = "default";
