open TestFramework;
open System;

let testDir = Path.mk(".test_cache");
let config = {...ConfigFile.dummy, sdkBaseDir: Some(testDir)};

%raw
"
jest.setTimeout(5000);
   ";

let testkey1 =
  "tz1ckaMTzEzzgYrPqXY4Y6dMxV38qf3LKJ5p"->PublicKeyHash.build->Result.getExn;

let testkey2 =
  "tz1iAmRoLmqhv4WekE4KkLjCUuv22Xq2MFoF"->PublicKeyHash.build->Result.getExn;

let testkey3 =
  "tz1LboVmDa9YdVR8S2LozRRoQkd26WFd6Dd8"->PublicKeyHash.build->Result.getExn;

let sktestkey3 = "edesk1mDurBchc7SUnBiWMWHuCFXvAmbwm5L6HFMrV9hMY9vefknezcMvPBshAhV1XyF5j9jWQGeB44ueZgdFVvF";

let recoveryPhrase5 = "update movie output save cream shoe word company never liar already wisdom save solar weather memory voice wagon grape tongue cement chair winner trumpet";

let testkey5 =
  "tz1UBwnNSFrvbPmtVT4vHCsba3mjMGtKhpu2"->PublicKeyHash.build->Result.getExn;

let bind = Future.flatMapOk;
let map = Future.mapOk;

let before = () => Client.initDir(testDir);

let after = () => Client.resetDir(testDir);

let futureEqual =
    ({expectEqual, callback}: Types.testAsyncUtils(_), eqcomp, fn) => {
  before()
  ->bind(fn)
  ->bind(v => after()->map(() => v))
  ->Future.get(v => {
      expectEqual(v, eqcomp);
      callback();
    });
};

describe("Wallet", ({testAsync}) => {
  open WalletAPI.Aliases;

  testAsync("Add alias", test =>
    test->futureEqual(Ok(Some(testkey1)), () =>
      add(~config, ~alias="testkey1", ~address=testkey1)
      ->bind(_ => getAddressForAlias(~config, ~alias="testkey1"))
    )
  );

  testAsync("Get from unknown alias", test =>
    test->futureEqual(Ok(None), () =>
      add(~config, ~alias="testkey1", ~address=testkey1)
      ->bind(_ => getAddressForAlias(~config, ~alias="testkey2"))
    )
  );

  testAsync("Get from address", test =>
    test->futureEqual(Ok(Some("testkey1")), () =>
      add(~config, ~alias="testkey1", ~address=testkey1)
      ->bind(_ => getAliasForAddress(~config, ~address=testkey1))
    )
  );

  testAsync("Get from unknown address", test =>
    test->futureEqual(Ok(None), () =>
      add(~config, ~alias="testkey1", ~address=testkey1)
      ->bind(_ => getAliasForAddress(~config, ~address=testkey2))
    )
  );

  testAsync("Delete", test => {
    let deleted = "testkey1";
    test->futureEqual(Ok(None), () =>
      add(~config, ~alias=deleted, ~address=testkey1)
      ->bind(() => add(~config, ~alias="testkey2", ~address=testkey2))
      ->bind(() => delete(~config, ~alias=deleted))
      ->bind(_ => getAddressForAlias(~config, ~alias=deleted))
    );
  });

  let getResult = [|
    ("testkey1", testkey1),
    ("testkey2", testkey2),
    ("testkey3", testkey3),
  |];

  testAsync("Get all", test =>
    test->futureEqual(Ok(getResult), () =>
      add(~config, ~alias="testkey1", ~address=testkey1)
      ->bind(() => add(~config, ~alias="testkey2", ~address=testkey2))
      ->bind(() => add(~config, ~alias="testkey3", ~address=testkey3))
      ->bind(_ => get(~config))
      ->map(a =>
          a->SortArray.stableSortBy(((v1, _), (v2, _)) => compare(v1, v2))
        )
    )
  );

  let renaming = {old_name: "testkey1", new_name: "testkey2"};

  testAsync("Rename", test =>
    test->futureEqual(Ok(Some(testkey1)), () =>
      add(~config, ~alias="testkey1", ~address=testkey1)
      ->bind(_ => rename(~config, renaming))
      ->bind(_ => getAddressForAlias(~config, ~alias="testkey2"))
    )
  );

  testAsync("Import", test =>
    test->futureEqual(Ok(Some(testkey3)), () =>
      WalletAPI.Accounts.import(
        ~config,
        ~alias="testimport",
        ~secretKey=sktestkey3,
        ~password="totototo",
      )
      ->bind(_ => getAddressForAlias(~config, ~alias="testimport"))
    )
  );

  testAsync("LegacyImport", test =>
    test->futureEqual(Ok(testkey5), () =>
      WalletAPI.Accounts.legacyImport(
        ~config,
        "zebra",
        recoveryPhrase5,
        ~password="totototo",
      )
    )
  );
});
