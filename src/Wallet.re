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

type error =
  | Generic(string);

module SecretAliases = {
  type alias = {
    name: string,
    value: string,
  };
  type t = array(alias);

  [@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";
};

module PkAliases = {
  type value = {
    locator: string,
    key: string,
  };

  type alias = {
    name: string,
    value,
  };
  type t = array(alias);

  [@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";
};

module PkhAliases = {
  type alias = {
    name: string,
    value: string,
  };
  type t = array(alias);

  [@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";
};

open System.Path.Ops;

let aliasFromPkh = (~dirpath, ~pkh, ()) => {
  System.File.read(dirpath / (!"public_key_hashs"))
  ->Future.mapError(e => Generic(e))
  ->Future.flatMapOk(file => {
      PkhAliases.parse(file)
      ->Js.Array2.find(a => a.value == pkh)
      ->FutureEx.fromOption(~error=Generic("No key found !"))
      ->Future.mapOk(a => a.PkhAliases.name)
    });
};

let pkFromAlias = (~dirpath, ~alias, ()) => {
  System.File.read(dirpath / (!"public_keys"))
  ->Future.mapError(e => Generic(e))
  ->Future.flatMapOk(file => {
      PkAliases.parse(file)
      ->Js.Array2.find(a => a.PkAliases.name == alias)
      ->FutureEx.fromOption(~error=Generic("No key found !"))
      ->Future.mapOk(a => a.PkAliases.value.key)
    });
};

type kind =
  | Encrypted
  | Unencrypted
  | Ledger;

let readSecret = (address, dirpath) =>
  aliasFromPkh(~dirpath, ~pkh=address, ())
  ->Future.flatMapOk(alias => {
      System.File.read(dirpath / (!"secret_keys"))
      ->Future.mapError(e => Generic(e))
      ->Future.flatMapOk(file => {
          SecretAliases.parse(file)
          ->Js.Array2.find(a => a.SecretAliases.name == alias)
          ->FutureEx.fromOption(~error=Generic("No key found !"))
          ->Future.mapOk(a => a.SecretAliases.value)
        })
    })
  ->Future.flatMapOk(key =>
      (
        key->Js.String2.startsWith("encrypted:")
          ? Ok((Encrypted, key))
          : key->Js.String2.startsWith("unencrypted:")
              ? Ok((Unencrypted, key))
              : key->Js.String2.startsWith("ledger://")
                  ? Ok((Ledger, key))
                  : Error(Generic("Can't readkey, bad format: " ++ key))
      )
      ->Future.value
    );
