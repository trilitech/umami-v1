%raw
"
/**
 * @jest-environment node
 */
// We need to force node environment bevause test brakes in jsdom environment
";

open TestFramework;
open System;

let testDir = Path.mk(".test_cache");
let config = {...ConfigContext.default, baseDir: () => testDir};

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

let bind = Promise.flatMapOk;
let map = Promise.mapOk;

let before = () => Client.initDir(testDir);

let after = () => Client.resetDir(testDir);

let futureEqual =
    ({expectEqual, callback}: Types.testAsyncUtils(_), eqcomp, fn) => {
  before()
  ->bind(fn)
  ->bind(v => after()->map(() => v))
  ->Promise.get(v => {
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

describe("Signer detection", ({test}) => {
  open Wallet;

  let skl1 = "ledger://tz1gr5TA8waD7LcrXNRaSz7Bys2Y14AWZnGH/ed25519/1h/0h";
  let skl1NoPref = "tz1gr5TA8waD7LcrXNRaSz7Bys2Y14AWZnGH/ed25519/1h/0h";

  let skl2 = "ledger://tz1gr5TA8waD7LcrXNRaSz7Bys2Y14AWZnGH/ed25519/0h/0h";
  let skl2NoPref = "tz1gr5TA8waD7LcrXNRaSz7Bys2Y14AWZnGH/ed25519/0h/0h";

  let ske = "ledge://test";
  let errbad = k => Error(Wallet.KeyBadFormat(k));

  let sken1 = "encrypted:edesk1eYwyEH1ofeBGC1HLNTAkhw2kM87FRvFg82Pkbi779evnVcW87Dx4VEtCw3dnhNSxsbWitpx6r7DRRLp4jY";
  let sken1NoPref = "edesk1eYwyEH1ofeBGC1HLNTAkhw2kM87FRvFg82Pkbi779evnVcW87Dx4VEtCw3dnhNSxsbWitpx6r7DRRLp4jY";
  let sken2 = "encrypted:edesk1K62pC53j338dwVGXdfgetvfjqBnQBvcaBsqMeSZzrqmfzGshDmMRZk7zfgbqvBPzvoC4dG9CZV6FXTGp9c";

  let sken2NoPref = "edesk1K62pC53j338dwVGXdfgetvfjqBnQBvcaBsqMeSZzrqmfzGshDmMRZk7zfgbqvBPzvoC4dG9CZV6FXTGp9c";

  let sks = [
    (skl1, Ok((Ledger, skl1NoPref))),
    (skl2, Ok((Ledger, skl2NoPref))),
    (ske, errbad(ske)),
    (sken1, Ok((Encrypted, sken1NoPref))),
    (sken2, Ok((Encrypted, sken2NoPref))),
  ];

  sks->List.forEachWithIndex((i, (inp, out)) => {
    test(
      "Parsing prefix n" ++ i->Int.toString,
      ({expectEqual}) => {
        let r = inp->Wallet.extractPrefixFromSecretKey;
        expectEqual(r, out);
      },
    )
  });
  open Wallet.Ledger;

  let skl1Res = {
    path: DerivationPath.buildTezosBip44((1, 0)),
    scheme: ED25519,
  };

  let sk1pkh =
    "tz1gr5TA8waD7LcrXNRaSz7Bys2Y14AWZnGH"->PublicKeyHash.build->Result.getExn;

  test("Parsing SK Ledger", ({expectEqual}) => {
    expectEqual(
      Ok(skl1Res),
      Decode.fromSecretKey(skl1NoPref, ~ledgerBasePkh=sk1pkh),
    )
  });
});
