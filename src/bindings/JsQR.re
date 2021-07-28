type inversionAttempts = [
  | `attemptBoth
  | `dontInvert
  | `onlyInvert
  | `invertFirst
];

[@bs.deriving abstract]
type options = {
  [@bs.optional]
  inversionAttempts,
};

type qrCode = {data: string};

[@bs.module "jsqr"]
external scan:
  (Js.TypedArray2.Uint8ClampedArray.t, float, float, options) =>
  Js.nullable(qrCode) =
  "default";
