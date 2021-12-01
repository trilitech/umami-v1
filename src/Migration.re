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

type Errors.t +=
  | MigrationFailed(Version.t);

let () =
  Errors.registerHandler(
    "LocalStorage",
    fun
    | MigrationFailed(v) =>
      I18n.errors#storage_migration_failed(Version.toString(v))->Some
    | _ => None,
  );

let currentVersion = Version.mk(1, 3);

let addMigration = (migrations, version, migration) => {
  migrations->Map.update(
    version,
    fun
    | None => [migration]->Some
    | Some(m) => [migration, ...m]->Some,
  );
};

let applyMigration = (migrations, currentVersion) => {
  migrations->Map.reduce(Ok(), (res, version, migrations) =>
    Version.compare(currentVersion, version) >= 0
      ? res
      : migrations
        ->List.reduce(res, (res, migration) =>
            res->Result.flatMap(_ => migration())
          )
        ->Result.mapError(_ => MigrationFailed(version))
  );
};

let init = version => {
  Map.make(~id=(module Version.Comparable))
  ->addMigration(Disclaimer.Legacy.V1_1.version, Disclaimer.Legacy.V1_1.mk)
  ->addMigration(ConfigFile.Legacy.V1_2.version, ConfigFile.Legacy.V1_2.mk)
  ->addMigration(
      TokenRegistry.Legacy.V1_3.version,
      TokenRegistry.Legacy.V1_3.mk,
    )
  ->applyMigration(version);
};
