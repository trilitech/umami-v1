/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

// Here Custom contains a name, not a chainId
type network = Network.configurableChains(string);

type t = {
  network: option(network),
  theme: option([ | `system | `dark | `light]),
  confirmations: option(int),
  sdkBaseDir: option(System.Path.t),
  customNetworks: list(Network.network),
  backupFile: option(System.Path.t),
  autoUpdates: bool,
};

[@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";

let parse = s => {
  let parseNetwork: [> network] => option(network) =
    fun
    | (`Mainnet | `Ithacanet | `Custom(_)) as v => Some(v)
    | _ => None;
  let c = s->parse;
  let network = c.network->Option.flatMap(parseNetwork);
  {...c, network};
};

let dummy = {
  network: None,
  theme: None,
  confirmations: None,
  sdkBaseDir: None,
  customNetworks: [],
  backupFile: None,
  autoUpdates: true,
};

module Encode = {
  open JsonEx.Encode;

  let themeToString =
    fun
    | `system => "system"
    | `dark => "dark"
    | `light => "light";

  let encoder = c =>
    object_([
      ("network", nullable(Network.Encode.chainEncoderString, c.network)),
      ("theme", nullable(string, c.theme->Option.map(themeToString))),
      ("confirmations", nullable(int, c.confirmations)),
      (
        "sdkBaseDir",
        nullable(string, c.sdkBaseDir->Option.map(System.Path.toString)),
      ),
      ("customNetworks", list(Network.Encode.encoder, c.customNetworks)),
      (
        "backupFile",
        nullable(string, c.backupFile->Option.map(System.Path.toString)),
      ),
      ("autoUpdates", bool(c.autoUpdates)),
    ]);
};

module Decode = {
  open JsonEx.Decode;

  let themeFromString =
    fun
    | "light" => Ok(`light)
    | "dark" => Ok(`dark)
    | "system" => Ok(`system)
    | s => Error(s);

  let networkDecoder = json =>
    json
    |> field(
         "network",
         optional(
           Network.Decode.(chainDecoderString(nativeChainFromString)),
         ),
       );

  let themeDecoder = json =>
    (json |> field("theme", optional(string)))
    ->Option.map(t => t->themeFromString->Result.getWithDefault(`system));

  let confirmationsDecoder = field("confirmations", optional(int));

  let autoUpdatesDecoder = field("autoUpdates", bool);

  let sdkBaseDirDecoder = json =>
    (json |> field("sdkBaseDir", optional(string)))
    ->Option.map(System.Path.mk);

  let customNetworksDecoder = json =>
    (
      json |> optional(field("customNetworks", list(Network.Decode.decoder)))
    )
    ->Option.getWithDefault([]);

  let backupFileDecoder = json =>
    (json |> optional(field("backupFile", string)))
    ->Option.map(System.Path.mk);

  let decoder = json => {
    network: json |> networkDecoder,
    theme: json |> themeDecoder,
    confirmations: json |> confirmationsDecoder,
    sdkBaseDir: json |> sdkBaseDirDecoder,
    customNetworks: json |> customNetworksDecoder,
    backupFile: json |> backupFileDecoder,
    autoUpdates: json |> autoUpdatesDecoder,
  };
};

module Storage =
  LocalStorage.Make({
    let key = "Config";
    type nonrec t = t;

    let encoder = Encode.encoder;
    let decoder = Decode.decoder;
  });

module Legacy = {
  module V1_2 = {
    open JsonEx.Decode;

    let removeNonNativeNetwork: [> network] => [ network] =
      fun
      | #Network.nativeChains as c => c
      | _ => `Mainnet;

    let networkVariantLegacyDecoder = json => {
      let embeddedNetworkDecoder = json =>
        json->string->Network.Decode.chainFromString;
      let customNetworkDecoder = json =>
        json
        |> (
          field("NAME", string)
          |> andThen(
               fun
               | "Custom" => field("VAL", Network.Decode.chainIdDecoder)
               | v =>
                 JsonEx.(
                   raise(
                     InternalError(DecodeError("Unknown variant " ++ v)),
                   )
                 ),
             )
        )
        |> (n => `Custom(n));

      json |> either(embeddedNetworkDecoder, customNetworkDecoder);
    };

    let networkLegacyDecoder = json =>
      (json |> optional(field("network", networkVariantLegacyDecoder)))
      ->Option.map(
          fun
          | `Custom((c: Network.chainId)) => `Custom((c :> string))
          | #Network.supportedChains as n => n,
        )
      ->Option.map(removeNonNativeNetwork);

    let legacyChainDecoder = json =>
      Network.{
        name: json |> field("name", string),
        chain: json |> field("chain", networkVariantLegacyDecoder),
        explorer: json |> field("explorer", string),
        endpoint: json |> field("endpoint", string),
      };

    let customNetworksLegacyDecoder = json =>
      (
        json
        |> optional(
             field("customNetworks", bsListDecoder(legacyChainDecoder)),
           )
      )
      ->Option.getWithDefault([]);

    let legacyDecoder = json =>
      Decode.{
        network: json |> networkLegacyDecoder,
        theme: json |> themeDecoder,
        confirmations: json |> confirmationsDecoder,
        sdkBaseDir: json |> sdkBaseDirDecoder,
        customNetworks: json |> customNetworksLegacyDecoder,
        backupFile: json |> backupFileDecoder,
        autoUpdates: json |> autoUpdatesDecoder,
      };

    let version = Version.mk(1, 2);
    let mk = () => {
      let mapValue = s => {
        JsonEx.parse(s)
        ->Result.flatMap(json => json->JsonEx.decode(legacyDecoder));
      };
      Storage.migrate(~mapValue, ~default=dummy, ());
    };
  };

  // This migration should be initiated the moment Granadanet is no longer available
  module V1_5 = {
    open JsonEx.Decode;
    open Decode;

    let legacyNativeChainFromString =
      fun
      | "Mainnet" => `Mainnet
      | "Ithacanet" => `Ithacanet
      | n =>
        JsonEx.(raise(InternalError(DecodeError("Unknown network " ++ n))));

    let legacyNetworkDecoder = json =>
      json
      |> field(
           "network",
           optional(
             Network.Decode.chainDecoderString(legacyNativeChainFromString),
           ),
         );

    let legacyDecoder = json => {
      network: json |> legacyNetworkDecoder,
      theme: json |> themeDecoder,
      confirmations: json |> confirmationsDecoder,
      sdkBaseDir: json |> sdkBaseDirDecoder,
      customNetworks: json |> customNetworksDecoder,
      backupFile: json |> backupFileDecoder,
      autoUpdates: json |> autoUpdatesDecoder,
    };

    let version = Version.mk(1, 5);
    let mk = () => {
      let mapValue = s => {
        JsonEx.parse(s)
        ->Result.flatMap(json => json->JsonEx.decode(legacyDecoder));
      };
      Storage.migrate(
        ~previousKey=Storage.key,
        ~mapValue,
        ~default=dummy,
        (),
      );
    };
  };
};

let write = s => Storage.set(s);

let read = () => Storage.get();

let reset = () => Storage.remove();
