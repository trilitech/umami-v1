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
  })
})
