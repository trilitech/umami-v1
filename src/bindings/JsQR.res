type inversionAttempts = [
  | #attemptBoth
  | #dontInvert
  | #onlyInvert
  | #invertFirst
]

@deriving(abstract)
type options = {
  @optional
  inversionAttempts: inversionAttempts,
}

type qrCode = {data: string}

@module("jsqr")
external scan: (Js.TypedArray2.Uint8ClampedArray.t, float, float, options) => Js.nullable<qrCode> =
  "default"
