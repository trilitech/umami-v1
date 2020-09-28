module Uint8Array = Js.Typed_array.Uint8Array;

[@bs.module "./crypto"] [@bs.val]
external edsk: Uint8Array.t => Js.Promise.t(string) = "edsk";