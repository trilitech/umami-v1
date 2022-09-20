open Umami
open Jest
open TestingLibraryHooks

open TestUtils

open Umami

let assertHistoryEqual = (
  ~show=Js.String.make,
  ~eq=\"=",
  finish: _ => unit,
  useHook,
  init,
  expHistory,
) => {
  let stateValue = renderHook(() => useHook(init))

  let failHistory = exp =>
    finish(
      fail(
        "history not equal: expected " ++
        (show(exp) ++
        (" <> " ++ show(stateValue.result.current))),
      ),
    )

  waitForReduce(expHistory, expHistory =>
    switch expHistory {
    | list{} =>
      finish(fail("empty history"))
      Stop
    | list{s} =>
      eq(s, stateValue.result.current) ? () : failHistory(s)
      Stop
    | list{s, ...expHistory} =>
      if eq(s, stateValue.result.current) {
        Continue(expHistory)
      } else {
        failHistory(s)
        Stop
      }
    }
  )->Promise.get(_ => finish(pass))
}

module Mock = {
  module Impl = {
    let useConfig = () => ConfigContext.fromFile(ConfigFile.dummy)
    let useAddLog = ((), _, _) => ()

    let useRetryNetwork = (
      (),
      ~onlyOn as _: Network.status=Online,
      ~onlyAfter as _: Network.status=Online,
      (),
    ) => ()
  }

  let requestResult = "my result"

  let get = (~config as _: Umami.ConfigContext.env, _) => {
    let mockFuture = Promise.make(resolve =>
      Js.Global.setTimeout(// setImmediate not binded I would use that
      () => ReactTestingLibrary.act(() => resolve(Ok(requestResult))), 1)
    )

    mockFuture
  }

  module ApiRequest = ApiRequest.Make(Impl)
}

let test = (useCallF, title, init, history) =>
  testAsync(title, ~timeout=10000, finish =>
    assertHistoryEqual(
      ~show=Mock.ApiRequest.toString,
      ~eq=ApiRequest.eq,
      finish,
      useCallF,
      init,
      history,
    )
  )

describe("useGetter", () => {
  let useCallF = init => {
    let (state, setState) = React.useState(() => init)
    let f = Mock.ApiRequest.useGetter(
      ~toast=false,
      ~get=Mock.get,
      ~kind=Account,
      ~setRequest=setState,
      (),
    )
    React.useEffect0(() => {
      f("mockedinput")->ignore
      None
    })

    state
  }

  let test = test(useCallF)

  test(
    "NotAsked: history should be the same",
    ApiRequest.NotAsked,
    list{Loading(None), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
  test(
    "Loading(Some): history should be the same",
    Loading(Some("something")),
    list{Loading(Some("something")), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
  test(
    "Loading(None): history should be the same",
    Loading(None),
    list{Loading(None), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
  test(
    "Done(Ok, Expired): history should be the same",
    Done(Ok("something"), Expired),
    list{Loading(Some("something")), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
  // this test pass but useGetter should be slightly rewritten to
  // not set state to loading if cache is still valid.
  test(
    "Done(Ok, ValidSince(<now>)): history should be the same",
    Done(Ok("something"), ApiRequest.initCache()),
    list{Loading(Some("something")), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
})

describe("useLoader", () => {
  let useCallF = init => {
    let requestState = React.useState(() => init)
    let state = ApiRequest.useLoader(~get=Mock.get, ~kind=Account, ~requestState, ())

    state
  }

  let test = test(useCallF)

  test(
    "NotAsked: history should be the same",
    ApiRequest.NotAsked,
    list{Loading(None), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
  test(
    "Loading(Some): history should be the same",
    Loading(Some("something")),
    list{Loading(Some("something")), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
  test("Loading(None): history should be the same", Loading(None), list{Loading(None)})
  test(
    "Done(Ok, Expired): history should be the same",
    Done(Ok("something"), Expired),
    list{Loading(Some("something")), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
  test(
    "Done(Ok, ValidSince(<now>)): history should be the same",
    Done(Ok("something"), ApiRequest.initCache()),
    list{Loading(Some("something")), Done(Ok(Mock.requestResult), ApiRequest.initCache())},
  )
})
