open TestFramework;

describe("Scan tests", ({test: _, testAsync}) => {
  testAsync("foo", ({expect, callback})
    => {
      HD.edesk(
        "m/44'/1729'/0'/0'"
        ->DerivationPath.Pattern.fromString
        ->Result.getExn
        ->DerivationPath.Pattern.implement(0),
        "zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra",
        ~password="blerot",
      )
      ->Future.get(_ => {
          Js.log("got something");
          expect.value("foo").toEqual("foo");
          callback();
        });
      ();
      //expect.value(key).toEqual(Some("edsk2tUyhVvGj9B1S956ZzmaU4bC9J7t8xVBH52fkAoZL25MHEwacd"));
    })
    /*
     test("runs valid derivation from seed test", ({expect}) => {
       let seed =
         HD.BIP39.mnemonicToSeedSync(
           "zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra",
         );
       let key: option(string) =
         switch (
           HD.toEDSK(HD.deriveFromSeed(seed, "m/44'/1729'/0'/0'"))
         ) {
         | value => Some(value)
         | exception _ => None
         };
       ();
       expect.value(key).toEqual(Some("edsk2tUyhVvGj9B1S956ZzmaU4bC9J7t8xVBH52fkAoZL25MHEwacd"));
     });

     test("runs inalid derivation from seed test", ({expect}) => {
       let seed =
         HD.BIP39.mnemonicToSeedSync(
           "zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra zebra",
         );
       let key: option(string) =
         switch (
           HD.toEDSK(HD.deriveFromSeed(seed, "invalid derivation path"))
         ) {
         | value => Some(value)
         | exception _ => None
         };
       ();
       expect.value(key).toEqual(None);
     });
     */
});
