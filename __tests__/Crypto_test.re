open TestFramework;
let tests = [
  (
    (
      "588d99e05da73a3301c3dc5d533f99db26cd63b3c380ae28193b87033b0b8b30",
      "fd9786d46667fe0ed1b52e72a0942d720b212578edc4d4a7e416699309f1dc5",
    ),
    "tz2JQAfC2QU1BKwswTyqRfgv3sTzWMZW5evQ"->PublicKeyHash.build,
  ),
  (
    (
      "222e9626c0a40d16d3155218986a9ab37ad18ce2cf1aa83dfd2dc0e5bb96000c",
      "1cba97ed8b46d852d426a72312452e586a14a7e7f4ac5ad7c05361259bc96c60",
    ),
    "tz2GPqHZXCfKRxuKTr6MMYZ4zUALU5MpeW83"->PublicKeyHash.build,
  ),
  (
    (
      "afc47959ae1f422168646bb18f830362763b7e2707d736230af50991b8792c2",
      "6f10275b009b5f0c58ee25ef3d66699fd316de64a4252f84ef50c2cd4159db51",
    ),
    "tz2GVJSWu5S9QtHyLPctZUXNumJBweokYnVS"->PublicKeyHash.build,
  ),
  (
    (
      "a9cd760ac50d2d50ae088bbd066fd33ef836efeb2528dd6ee61b4a04cea96e47",
      "1fba99e7885afccd874706c3856ad14b5a388374b48d6f239bd17debe3ca522a",
    ),
    "tz2Sc2UB33hExdHphfSWKPZ5LTbDuKHneKrW"->PublicKeyHash.build,
  ),
];

describe("Crypto", ({test}) => {
  tests->List.forEachWithIndex((i, ((x, y), out)) => {
    test(
      "Points to Pkh: should be equal n" ++ i->Int.toString, ({expectEqual}) => {
      expectEqual(Crypto.spPointsToPkh(~x, ~y), out)
    })
  });
  test("Pk to pkh: should be equal", ({expectEqual}) => {
    expectEqual(
      Crypto.pk2pkh("edpkuxA6k6tR7RNLSSh6tyJnSEqzvLUMgS9z5cawUFpzJrUu26g6rN"),
      "tz1ckaMTzEzzgYrPqXY4Y6dMxV38qf3LKJ5p"->Ok,
    )
  });
});
