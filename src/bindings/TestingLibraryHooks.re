type current('a) = {current: 'a};
type renderHookResult('hookReturnType) = {result: current('hookReturnType)};

[@bs.module "@testing-library/react-hooks"]
external renderHook: ('a => 'b) => renderHookResult('b) = "renderHook";

[@bs.module "@testing-library/react-hooks"]
external act: (unit => 'a) => unit = "act";
