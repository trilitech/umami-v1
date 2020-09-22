module Uint8Array = Js.Typed_array.Uint8Array;

[@bs.module "bip39"] [@bs.val]
external generateMnemonic: int => string = "generateMnemonic";
[@bs.module "bip39"] [@bs.val]
external mnemonicToSeedSync: string => Uint8Array.t = "mnemonicToSeedSync";
[@bs.module "bip39"] [@bs.val]
external mnemonicToSeed: string => Js.Promise.t(Uint8Array.t) =
  "mnemonicToSeed";