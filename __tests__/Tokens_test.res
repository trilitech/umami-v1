open TestFramework
open System
open TokenRepr

let testDir = Path.mk(".test_cache")
let config = {...ConfigContext.default, baseDir: () => testDir}

let token1 = {
  kind: FA1_2,
  address: "KT1F8BQQcAbs9LthTMsFMAHs2L2QN5wnaFgL"->PublicKeyHash.build->Result.getExn,
  alias: "Token1",
  symbol: "TK1",
  chain: #Granadanet->Network.getChainId,
  decimals: 2,
  asset: Metadata.defaultAsset,
}

let token2 = {
  kind: FA2(0),
  address: "KT1HQfgU4g3sQQVTKepxh9yT5gyQLSJtfpvz"->PublicKeyHash.build->Result.getExn,
  alias: "Token2",
  symbol: "TK2",
  chain: #Granadanet->Network.getChainId,
  decimals: 10,
  asset: {
    open Metadata
    {
      ...defaultAsset,
      blocklevel: Some(12934),
      isBooleanAmount: false,
      description: Some("Undestructible NFT"),
      isTransferable: false,
      formats: None,
      attributes: Some([{name: "kind", value: "non_transferable", type_: None}]),
    }
  },
}

let assetJson = `
  {
    "blocklevel": 12934,
    "isBooleanAmount": false,
    "description": "Undestructible NFT",
    "isTransferable": false,
    "formats": null,
    "attributes": [{"name": "kind", "value":"non_transferable"}]
  }
`

let token1Json = `
   {
     "kind": "fa1-2",
     "address": "KT1F8BQQcAbs9LthTMsFMAHs2L2QN5wnaFgL",
     "alias": "Token1",
     "symbol": "TK1",
     "chain": "NetXz969SFaFn8k",
     "decimals": 2,
     "asset": {}
   }
`

let token2Json = Format.sprintf(
  `
     {
       "kind": "fa2",
       "tokenId": 0,
       "address": "KT1HQfgU4g3sQQVTKepxh9yT5gyQLSJtfpvz",
       "alias": "Token2",
       "symbol": "TK2",
       "chain": "NetXz969SFaFn8k",
       "decimals": 10,
       "asset": %s
     }
`,
  assetJson,
)

describe("Tokens", ({test}) => {
  test("Parse FA1.2", ({expectEqual}) => {
    let token =
      token1Json->JsonEx.parse->Result.flatMap(json => json->JsonEx.decode(Token.Decode.record))
    expectEqual(token, Ok(token1))
  })

  test("Parse FA2", ({expectEqual}) => {
    let token =
      token2Json->JsonEx.parse->Result.flatMap(json => json->JsonEx.decode(Token.Decode.record))
    expectEqual(token, Ok(token2))
  })

  test("Encode FA1.2", ({expectEqual}) => {
    let token1Encoded = token1->Token.Encode.record
    let token1' = token1Encoded->JsonEx.decode(Token.Decode.record)
    expectEqual(token1', Ok(token1))
  })

  test("Encode FA2", ({expectEqual}) => {
    let token2Encoded = token2->Token.Encode.record
    let token2' = token2Encoded->JsonEx.decode(Token.Decode.record)
    expectEqual(token2', Ok(token2))
  })
})

let exampleWithEscapedString = `
[["KT1HTTTjKuzxrwPMqHfxkppedffzn9uVKy51",{"name":"zzz'\\\\"<p>{{2*2}}","address":"KT1HTTTjKuzxrwPMqHfxkppedffzn9uVKy51","tokens":[{"kind":"full","value":{"kind":"fa2","address":"KT1HTTTjKuzxrwPMqHfxkppedffzn9uVKy51","alias":"xxxzzz\\\\"<p>{{2*2}}","symbol":"OBJKTCOM","chain":"NetXdQprcVkpaWU","decimals":0,"tokenId":2,"asset":{"description":"xxxzzz\\\\"<p>{{2*2}}","minter":null,"creators":["zzz\\\\"<p>{{2*2}}"],"contributors":null,"publishers":null,"date":"2022-04-28T21:38:34.380Z","blocklevel":null,"type":null,"tags":["xxx"],"genres":null,"language":null,"identifier":null,"rights":"CC-BYzzz\\\\"<p>{{2*2}}","rightUri":null,"artifactUri":"ipfs://QmZ7UV3rN6zLzPfWzYxmZjn54D6UcyumLTmuYp6H8V4PZi","displayUri":"ipfs://Qme8uzbJJTTcxMkw6tscakTKMns5a1qU4J2xynLN4wbAb5","thumbnailUri":"ipfs://QmdmmjqLbPVqj6C35cofMgh5WmDWLgTvULMJnRMdodKJMo","isTransferable":true,"isBooleanAmount":false,"shouldPreferSymbol":false,"formats":[{"uri":"ipfs://QmZ7UV3rN6zLzPfWzYxmZjn54D6UcyumLTmuYp6H8V4PZi","hash":null,"mimeType":"image/png","fileSize":1639,"fileName":"zzz.png","duration":null,"dimensions":{"value":"511x376","unit":"px"},"dataRate":null},{"uri":"ipfs://Qme8uzbJJTTcxMkw6tscakTKMns5a1qU4J2xynLN4wbAb5","hash":null,"mimeType":"image/png","fileSize":913,"fileName":"cover-zzz.png","duration":null,"dimensions":{"value":"511x376","unit":"px"},"dataRate":null},{"uri":"ipfs://QmdmmjqLbPVqj6C35cofMgh5WmDWLgTvULMJnRMdodKJMo","hash":null,"mimeType":"image/png","fileSize":524,"fileName":"thumbnail-zzz.png","duration":null,"dimensions":{"value":"350x258","unit":"px"},"dataRate":null}],"attributes":[]}}}]}]]
`

describe("Parse token with escape string and wrong ", ({test}) =>
  test("Parsing should succeed", ({expectTrue}) => {
    let token =
      exampleWithEscapedString
      ->JsonEx.parse
      ->Result.flatMap(json => json->JsonEx.decode(TokenStorage.Cache.decoder))
    expectTrue(token->Result.isOk)
  })
)
