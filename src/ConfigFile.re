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

open Let;

type network = Network.configurableChains;

type t = {
  network: option(network),
  theme: option([ | `system | `dark | `light]),
  confirmations: option(int),
  sdkBaseDir: option(System.Path.t),
  customNetworks: list(Network.network),
};

[@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";

let parse = s => {
  let parseNetwork: [> network] => option(network) =
    fun
    | (`Mainnet | `Hangzhounet | `Granadanet | `Custom(_)) as v => Some(v)
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
      ("network", nullable(Network.Encode.chainEncoder, c.network)),
      ("theme", nullable(string, c.theme->Option.map(themeToString))),
      ("confirmations", nullable(int, c.confirmations)),
      (
        "sdkBaseDir",
        nullable(string, c.sdkBaseDir->Option.map(System.Path.toString)),
      ),
      ("customNetworks", list(Network.Encode.encoder, c.customNetworks)),
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
         optional(Network.Decode.(chainDecoder(nativeChainFromString))),
       );

  let themeDecoder = json =>
    (json |> field("theme", optional(string)))
    ->Option.map(t => t->themeFromString->Result.getWithDefault(`system));

  let confirmationsDecoder = field("confirmations", optional(int));

  let sdkBaseDirDecoder = json =>
    (json |> field("sdkBaseDir", optional(string)))
    ->Option.map(System.Path.mk);

  let customNetworksDecoder = json =>
    (
      json |> optional(field("customNetworks", list(Network.Decode.decoder)))
    )
    ->Option.getWithDefault([]);

  let decoder = json => {
    network: json |> networkDecoder,
    theme: json |> themeDecoder,
    confirmations: json |> confirmationsDecoder,
    sdkBaseDir: json |> sdkBaseDirDecoder,
    customNetworks: json |> customNetworksDecoder,
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
               | "Custom" => field("VAL", string)
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
      };

    let version = Version.mk(1, 2);
    let mk = () => {
      let mapValue = s => {
        let%Res json = JsonEx.parse(s);
        json->JsonEx.decode(legacyDecoder);
      };
      Storage.migrate(~mapValue, ~default=dummy, ());
    };
  };

  // This migration should be initiated the moment Granadanet is no longer available
  module VX_X = {
    open JsonEx.Decode;
    open Decode;

    let legacyNativeChainFromString =
      fun
      | "Mainnet" => `Mainnet
      | "Granadanet" => `Hangzhounet
      | n =>
        JsonEx.(raise(InternalError(DecodeError("Unknown network " ++ n))));

    let legacyNetworkDecoder = json =>
      json
      |> field(
           "network",
           optional(
             Network.Decode.chainDecoder(legacyNativeChainFromString),
           ),
         );

    let legacyDecoder = json => {
      network: json |> legacyNetworkDecoder,
      theme: json |> themeDecoder,
      confirmations: json |> confirmationsDecoder,
      sdkBaseDir: json |> sdkBaseDirDecoder,
      customNetworks: json |> customNetworksDecoder,
    };

    let version = Version.mk(1, 4);
    let mk = () => {
      let mapValue = s => {
        let%Res json = JsonEx.parse(s);
        json->JsonEx.decode(legacyDecoder);
      };
      Storage.migrate(~mapValue, ~default=dummy, ());
    };
  };
};

let write = s => Storage.set(s);

let read = () => Storage.get();

let reset = () => Storage.remove();
