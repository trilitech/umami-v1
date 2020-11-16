module Buffer = Js.TypedArray2.Uint8Array;

[@bs.send] external setArrayAt: (Buffer.t, Buffer.t, int) => unit = "set";
[@bs.send] external toHex: (Buffer.t, [@bs.as "hex"] _) => string = "toString";

[@bs.val] [@bs.scope "Buffer"] external toNodeBuffer: Buffer.t => Node_buffer.t = "from";

[@bs.val] [@bs.scope "crypto"]
external getRandomValues: Buffer.t => Buffer.t = "getRandomValues";

let make = size => Buffer.fromBuffer(Js.TypedArray2.ArrayBuffer.make(size));

let filledWithRandomValues = size => getRandomValues(make(size));