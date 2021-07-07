// from https://github.com/yamadayuki/bs-url
// with nullable fix

type t;

module SearchParams = {
  type t;

  [@bs.send.pipe: t] external get: string => Js.nullable(string) = "get";
};

[@bs.module "url"] [@bs.new] external make: string => t = "URL";

/* Members */
[@bs.get] external getSearchParams: t => SearchParams.t = "searchParams";
