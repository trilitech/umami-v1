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

type Errors.t +=
  | UnknownVersion(Version.t)
  | CannotParseVersion(string);

let () =
  Errors.registerHandler(
    "BackupFile",
    fun
    | UnknownVersion(v) =>
      v->Version.toString->I18n.Errors.unknown_backup_version->Some
    | CannotParseVersion(err) => err->I18n.Errors.cannot_parse_version->Some
    | _ => None,
  );

let currentVersion = Version.mk(1, 0);

type t = {
  version: Version.t,
  derivationPaths: array(DerivationPath.Pattern.t),
  recoveryPhrases: array(SecureStorage.Cipher.encryptedData),
};

let make = (~derivationPaths, ~recoveryPhrases) => {
  version: currentVersion,
  derivationPaths,
  recoveryPhrases,
};

let dummy = make(~derivationPaths=[||], ~recoveryPhrases=[||]);

let encoder = c =>
  JsonEx.Encode.object_([
    ("version", c.version->Version.toString->Json.Encode.string),
    (
      "derivationPaths",
      c.derivationPaths->Array.map(DerivationPath.Pattern.toString)
      |> Json.Encode.(array(string)),
    ),
    (
      "recoveryPhrases",
      c.recoveryPhrases |> Json.Encode.array(SecureStorage.Cipher.encoder),
    ),
  ]);

let decoder = json =>
  Json.Decode.{
    version: json |> field("version", LocalStorage.Version.decoder),
    derivationPaths:
      (json |> field("derivationPaths", Json.Decode.(array(string))))
      ->Array.map(DerivationPath.Pattern.fromString)
      ->Array.map(Result.getExn),
    recoveryPhrases:
      json
      |> field(
           "recoveryPhrases",
           Json.Decode.(array(SecureStorage.Cipher.decoder)),
         ),
  };

let checkVersion = json => {
  let%Res v =
    json->JsonEx.decode(
      Json.Decode.field("version", LocalStorage.Version.decoder),
    );
  Version.checkVersion(~current=v, ~expected=currentVersion)
  ->Result.mapError(
      fun
      | Version.UnknownVersion({current}) => UnknownVersion(current)
      | e => e->Errors.toString->CannotParseVersion,
    );
};

let read = (path: System.Path.t) => {
  let%AwaitRes file = System.File.read(path);
  let%Res json = JsonEx.parse(file);
  let%Res () = checkVersion(json);
  json->JsonEx.decode(decoder);
};

let save = (t, path: System.Path.t) => {
  let encoded = encoder(t);
  System.File.protect(~name=path, ~transaction=_ =>
    System.File.write(~name=path, encoded->JsonEx.stringify)
  );
};