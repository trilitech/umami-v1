open Jest
open ReactTestingLibrary

open ExpectJs

let mockFn = JestJs.fn(() => ())

let fixture =
  <Portal.Provider>
    <RefreshButton
      onRefresh={() => {
        mockFn->MockJs.fn()
        ()
      }}
    />
  </Portal.Provider>

describe("<RefreshButton />", () => {
  test("it displays one Button", () => {
    let screen = render(fixture)

    // Below expression throws if exactly one element with role button is not found
    // Adding expect just for to have an assetion
    let btn = screen->getByRole(~matcher=#Str("button"))

    btn->expect->toBeTruthy
  })

  test("clicking on button calls handler", () => {
    let screen = render(fixture)
    let btn = screen->getByRole(~matcher=#Str("button"))
    FireEvent.click(btn)

    let calls = mockFn->MockJs.calls
    expect(calls)->toEqual([()])
  })
})
