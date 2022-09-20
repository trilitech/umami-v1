open Jest

open ExpectJs

open Umami.Protocol
open Umami.Protocol.Amount
open Umami.Protocol.Transfer

let mockHash = "tz1TmMQ6MzRfB4bjnxJgfJ7YCN6uoY9ZVEzZ"
let makePkHash = str => str->PublicKeyHash.build->Result.getExn
let makeTokenAmount = amount => amount->TokenRepr.Unit.fromNatString->Result.getExn

let dummyParam = ProtocolOptions.makeParameter()

let mockAddress = makePkHash(mockHash)
let mockAlias: FormUtils.Alias.t = Address(mockAddress)

let makeMockAddress = add => makePkHash(add)
let makeMockAlias = (s): FormUtils.Alias.t => s->makeMockAddress->Address

let mockAccount: Account.t = {
  name: "senderAccountName",
  kind: Encrypted,
  address: mockAddress,
}

let makeMockTezAmount = n => Tez(Tez.fromMutezInt(n))

let mockTezAmount: Protocol.Amount.t = makeMockTezAmount(1234)

let makeMockTezAmount = n => Tez(Tez.fromMutezInt(n))

let makeMockFA2Token = (amount, address) => {
  let mockTokenAmount = makeTokenAmount(amount)

  let mockFA1_2Token: TokenRepr.t = {
    kind: FA2(33),
    address: makeMockAddress(address),
    alias: "foo",
    symbol: "bar",
    chain: Network.getChainId(#Mainnet),
    decimals: 2,
    asset: Metadata.defaultAsset,
  }
  let mockToken = {token: mockFA1_2Token, amount: mockTokenAmount}
  mockToken
}

let makeFA2ValidState = (~token, ~recipient) => {
  let mockValidState: SendForm.validState = {
    amount: Token(token),
    sender: mockAccount,
    recipient: recipient,
    parameter: None,
    entrypoint: None,
  }

  mockValidState
}

describe("GlobalBatchUtils", () => {
  test(
    "GlobalBatchUtils.addToExistingOrNew returns a new batch containing a manager (FA2 token case)",
    () => {
      let mockTokenAmount = makeTokenAmount("33")
      let mockFA1_2Token: TokenRepr.t = {
        kind: FA2(33),
        address: mockAddress,
        alias: "foo",
        symbol: "bar",
        chain: Network.getChainId(#Mainnet),
        decimals: 2,
        asset: Metadata.defaultAsset,
      }
      let mockToken: Protocol.Amount.t = Token({token: mockFA1_2Token, amount: mockTokenAmount})
      let transferFA2: Transfer.transferFA2 = {
        tokenId: 33,
        content: {
          destination: mockAddress,
          amount: {
            token: mockFA1_2Token,
            amount: mockTokenAmount,
          },
        },
      }
      let batchFA2: Transfer.batchFA2 = {
        address: mockAddress,
        transfers: list{transferFA2},
      }
      let options = ProtocolOptions.make()
      let parameter = dummyParam
      let manager: Protocol.manager = Transfer({
        options: options,
        data: FA2Batch(batchFA2),
        parameter: parameter,
      })
      let expected: Umami.Protocol.batch = {
        source: mockAccount,
        managers: [manager],
      }
      let result = GlobalBatchUtils.addToExistingOrNew(
        mockAccount,
        None,
        (mockToken, mockAlias->FormUtils.Alias.address, dummyParam),
      )
      expect(result)->toEqual(expected)
    },
  )

  test("GlobalBatchUtils.add adds transfer info to an existing batch", () => {
    let manager =
      ProtocolHelper.Transfer.makeSimple(
        ~data={destination: mockAddress, amount: mockTezAmount},
        (),
      )->Transfer
    let tezAmount2 = makeMockTezAmount(3)
    let manager2 =
      ProtocolHelper.Transfer.makeSimple(
        ~data={destination: mockAddress, amount: tezAmount2},
        (),
      )->Transfer
    let batchWithATezTransaction = {
      source: mockAccount,
      managers: [manager],
    }
    let result = GlobalBatchUtils.add(
      ~payload=(tezAmount2, mockAddress, dummyParam),
      ~batch=batchWithATezTransaction,
    )
    let expected = {source: mockAccount, managers: [manager, manager2]}
    expect(result)->toEqual(expected)
  })

  test("batchToIndexedRows returns indexed transfer infos", () => {
    let mockRecipient = mockAddress
    let mockTez = makeMockTezAmount(3344)
    let tezManager =
      GlobalBatchXfs.transferPayloadToTransferData((mockTez, mockRecipient, dummyParam))
      ->GlobalBatchXfs.transferDataToTransfer
      ->Transfer
    let batch = {source: mockAccount, managers: [tezManager]}
    let result = GlobalBatchXfs.batchToIndexedRows(batch)
    expect(result)->toEqual([((0, None), (makeMockTezAmount(3344), mockRecipient, dummyParam))])
  })
})
