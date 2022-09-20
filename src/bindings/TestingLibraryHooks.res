type current<'a> = {current: 'a}
type renderHookResult<'hookReturnType> = {result: current<'hookReturnType>}

@module("@testing-library/react-hooks")
external renderHook: ('a => 'b) => renderHookResult<'b> = "renderHook"

@module("@testing-library/react-hooks")
external act: (unit => 'a) => unit = "act"
