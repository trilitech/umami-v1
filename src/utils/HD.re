module Buffer = Js.TypedArray2.Uint8Array;

module BufferEx {
  type t = Buffer.t;

  [@bs.send] external setArrayAt: (t, t, int) => unit = "set";
};

[@bs.val] [@bs.scope "Buffer"] external nodeBufferfromBytes: Buffer.t => Node_buffer.t = "from";

[@bs.send] external toHex: (Buffer.t, [@bs.as "hex"] _) => string = "toString";

module ED25519 = {
  type derivedPath = {
    key: Buffer.t,
    chaincode: Buffer.t
  };

  [@bs.module "ed25519-hd-key"]
  external derivePath: (string, string) => derivedPath = "derivePath";
};

module BIP39 = {
  [@bs.module "bip39"]
  external mnemonicToSeedSync: string => Buffer.t = "mnemonicToSeedSync";
};

let deriveSeed = (seed, derivationPath) => {
  let derivedPath = ED25519.derivePath(derivationPath, seed->toHex);
  derivedPath.key;
};

module BS58Check = {
  [@bs.module "bs58check"] external encode: Node_buffer.t => string = "encode";
};

let b58cencode = (data, prefix) => {
  let buffer =
    Buffer.fromBuffer(
      Js.TypedArray2.ArrayBuffer.make(
        prefix->Js.Array2.length + data->Buffer.length,
      ),
    );
  buffer->Buffer.setArray(prefix);
  buffer->BufferEx.setArrayAt(data, prefix->Js.Array2.length)
  BS58Check.encode(nodeBufferfromBytes(buffer));
};

let seedToPrivateKey = seed => {
  b58cencode(
    seed->Buffer.slice(~start=0, ~end_=32),
    [|13, 15, 58, 7|] // edsk2
  );
};

let getMainDerivationPath = index => {
  {j|m/44'/1729'/$(index)'/0'|j};
};

let seedToHDPrivateKey = (seed, index) => {
  seedToPrivateKey(deriveSeed(seed, getMainDerivationPath(index)));
};
