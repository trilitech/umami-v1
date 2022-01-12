open Jest;
open Expect;
open TestingLibraryHooks;

let useCounter = () => {
  let (count, setCount) = React.useState(_ => 0);

  let increment = () => {
    setCount(x => x + 1);
  };

  (count, increment);
};

describe("Renderhook", () => {
  test("it allows us to test hook behavior", () => {
    let {result} = renderHook(useCounter);
    act(() => {
      let (_, increment) = result.current;
      increment();
    });
    let (count, _) = result.current;
    expect(count)->toBe(1);
  })
});
