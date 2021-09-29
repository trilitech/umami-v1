open TestFramework;
open Let;
open DerivationPath;

let tests = [
  (("m/44'/1729'/1'/1'", 0), Error(MissingWildcardOr0)),
  (("m/44'/1729'/1'/1'", 1), Error(MissingWildcardOr0)),
  (("m/44'/1729'/0'/1'", 0), Ok("m/44'/1729'/0'/1'")),
  (("m/44'/1729'/0'/1'", 1), Ok("m/44'/1729'/1'/1'")),
  (("m/44'/1729'/?'/1'", 0), Ok("m/44'/1729'/0'/1'")),
  (("m/44'/1729'/?'/1'", 1), Ok("m/44'/1729'/1'/1'")),
  (("m/44'/1729'/0'/0'", 0), Ok("m/44'/1729'/0'/0'")),
];

let impl = (pat, v) => {
  let%ResMap path = pat->Pattern.fromString;
  path->Pattern.implement(v)->toString;
};

let toString =
  fun
  | Ok(path) => path
  | Error(_) => "error";

describe("DerivationPath", ({test}) => {
  tests->List.forEachWithIndex((i, ((pat, v), out)) => {
    test("Output should be equal n" ++ i->Int.toString, ({expectEqual}) => {
      expectEqual(impl(pat, v), out)
    })
  })
});
