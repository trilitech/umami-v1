open TestFramework;

describe("Scan tests", ({test}) => {
  test("runs valid derivation path test", ({expect}) => {
    let seed =
      HD.BIP39.mnemonicToSeedSync(
        "zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra",
      );
    let key: option(string) =
      switch (
        HD.seedToPrivateKey(HD.deriveSeed(seed, "m/44'/1729'/0'/0'"))
      ) {
      | value => Some(value)
      | exception _ => None
      };
    ();
    expect.value(key).toEqual(Some("edsk2tUyhVvGj9B1S956ZzmaU4bC9J7t8xVBH52fkAoZL25MHEwacd"));
  });

  test("runs inalid derivation path test", ({expect}) => {
    let seed =
      HD.BIP39.mnemonicToSeedSync(
        "zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra",
      );
    let key: option(string) =
      switch (
        HD.seedToPrivateKey(HD.deriveSeed(seed, "invalid derivation path"))
      ) {
      | value => Some(value)
      | exception _ => None
      };
    ();
    expect.value(key).toEqual(None);
  });
});
