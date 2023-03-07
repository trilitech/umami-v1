open Jest
open ExpectJs

describe("Multisig", () => {
  describe("parseActions", () => {
    test("simple tez transactions are parsed properly", () => {
      open Operation
      open Operation.Transaction
      let input = `[{"prim":"DROP"},{"prim":"NIL","args":[{"prim":"operation"}]},{"prim":"PUSH","args":[{"prim":"key_hash"},{"bytes":"005fd0a7ece135cecfd71fcf78cf6656d5047fb980"}]},{"prim":"IMPLICIT_ACCOUNT"},{"prim":"PUSH","args":[{"prim":"mutez"},{"int":"3000000"}]},{"prim":"UNIT"},{"prim":"TRANSFER_TOKENS"},{"prim":"CONS"}]`
      let destination = PublicKeyHash.unsafeBuild("tz1UNer1ijeE9ndjzSszRduR3CzX49hoBUB3")
      let result = Multisig.API.parseActions(input)

      let expected = [
        Transaction(
          Tez({
            amount: Tez.fromMutezInt(3000000),
            destination: destination,
            entrypoint: None,
            parameters: None,
          }),
        ),
      ]->Some

      expect(result)->toEqual(expected)
    })

    test("KL2 transactions are parsed properly", () => {
      open Operation
      open Operation.Transaction
      let input = `[{"prim":"DROP"},{"prim":"NIL","args":[{"prim":"operation"}]},{"prim":"PUSH","args":[{"prim":"address"},{"bytes":"01fc1beb979d7c8da00d6e5e22c297bcf541834607007472616e73666572"}]},{"prim":"CONTRACT","args":[{"prim":"list","args":[{"prim":"pair","args":[{"prim":"address"},{"prim":"list","args":[{"prim":"pair","args":[{"prim":"address"},{"prim":"pair","args":[{"prim":"nat"},{"prim":"nat"}]}]}]}]}]}]},[{"prim":"IF_NONE","args":[[[{"prim":"UNIT"},{"prim":"FAILWITH"}]],[]]}],{"prim":"PUSH","args":[{"prim":"mutez"},{"int":"0"}]},{"prim":"PUSH","args":[{"prim":"list","args":[{"prim":"pair","args":[{"prim":"address"},{"prim":"list","args":[{"prim":"pair","args":[{"prim":"address"},{"prim":"pair","args":[{"prim":"nat"},{"prim":"nat"}]}]}]}]}]},[{"prim":"Pair","args":[{"bytes":"017528f2a0740b5633a81b18d10911111dc0f3bc8700"},[{"prim":"Pair","args":[{"bytes":"00005fd0a7ece135cecfd71fcf78cf6656d5047fb980"},{"prim":"Pair","args":[{"int":"0"},{"int":"300000"}]}]}]]}]]},{"prim":"TRANSFER_TOKENS"},{"prim":"CONS"}]`
      let destination = PublicKeyHash.unsafeBuild("tz1UNer1ijeE9ndjzSszRduR3CzX49hoBUB3")
      let fa2Contract = "KT1XZoJ3PAidWVWRiKWESmPj64eKN7CEHuWZ"->PublicKeyHash.unsafeBuild
      let result = Multisig.API.parseActions(input)

      let expected = [
        Transaction(
          Token(
            {
              amount: Tez.zero,
              destination: destination,
              entrypoint: None,
              parameters: None,
            },
            {
              kind: FA2(0),
              amount: ReBigNumber.fromInt(300000)->Obj.magic,
              contract: fa2Contract,
            },
          ),
        ),
      ]->Some

      expect(result)->toEqual(expected)
    })
  })
})
