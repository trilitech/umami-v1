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

[@bs.val] [@bs.scope "localStorage"]
external getItem: string => Js.Nullable.t(string) = "getItem";
[@bs.val] [@bs.scope "localStorage"]
external setItem: (string, string) => unit = "setItem";
[@bs.val] [@bs.scope "localStorage"]
external removeItem: string => unit = "removeItem";

[@bs.val] [@bs.scope "localStorage"] external clear: unit => unit = "clear";

type Errors.t +=
  | NotFound(string);

let () =
  Errors.registerHandler(
    "LocalStorage",
    fun
    | NotFound(s) => I18n.Errors.local_storage_key_not_found(s)->Some
    | _ => None,
  );

module type ValueType = {
  let key: string;

  type t;

  let encoder: Json.Encode.encoder(t);
  let decoder: Json.Decode.decoder(t);
};

module type StorageType = {
  include ValueType;

  let get: unit => Promise.result(t);
  let set: t => unit;
  let remove: unit => unit;
  let migrate:
    (
      ~previousKey: string=?,
      ~mapValue: string => Promise.result(t)=?,
      ~default: t=?,
      unit
    ) =>
    Promise.result(unit);
};

module Make = (Value: ValueType) : (StorageType with type t = Value.t) => {
  include Value;

  let getRaw = key =>
    getItem(key)
    ->Js.Nullable.toOption
    ->Result.fromOption(NotFound(key))
    ->Result.flatMap(JsonEx.parse)
    ->Result.flatMap(json => json->JsonEx.decode(decoder));

  let setRaw = (key, storage) =>
    setItem(key, encoder(storage)->Json.stringify);

  let get = () => getRaw(key);

  let set = storage => setRaw(key, storage);

  let remove = () => removeItem(key);

  let migrate = (~previousKey=?, ~mapValue=?, ~default=?, ()) => {
    let migrateValue = key =>
      switch (mapValue) {
      // ensures the previous value is compatible if we simply change the key
      | None => getRaw(key)

      | Some(migrate) =>
        getItem(key)
        ->Js.Nullable.toOption
        ->Result.fromOption(NotFound(key))
        ->Result.flatMap(migrate)
      };

    let useDefault = res =>
      switch (res, default) {
      | (Ok(_), _)
      | (_, None) => res
      | (Error(_), Some(v)) => Ok(v)
      };

    previousKey
    ->Option.getWithDefault(key)
    ->migrateValue
    ->useDefault
    ->Result.map(set);
  };
};

module Version =
  Make({
    let key = "storage-version";
    type t = Version.t;
    let encoder = v => Json.Encode.(v->Version.toString->string);
    let decoder = json => {
      json->Json.Decode.string->Version.parse->JsonEx.getExn;
    };
  });
