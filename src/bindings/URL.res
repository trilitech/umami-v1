// from https://github.com/yamadayuki/bs-url
// with nullable fix

type t

module SearchParams = {
  type t

  @send external get: (t, string) => Js.nullable<string> = "get"
}

@module("url") @new external make: string => t = "URL"

/* Members */
@get external getSearchParams: t => SearchParams.t = "searchParams"
