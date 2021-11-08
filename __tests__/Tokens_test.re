open TestFramework;
open System;
open TokenRepr;

let testDir = Path.mk(".test_cache");
let config = {...ConfigContext.default, baseDir: () => testDir};

let token1 = {
  kind: FA1_2,
  address:
    "KT1F8BQQcAbs9LthTMsFMAHs2L2QN5wnaFgL"->PublicKeyHash.build->Result.getExn,
  alias: "Token1",
  symbol: "TK1",
  chain: "NetXz969SFaFn8k",
  decimals: 2,
  asset: defaultAsset,
};

let token2 = {
  kind: FA2(0),
  address:
    "KT1HQfgU4g3sQQVTKepxh9yT5gyQLSJtfpvz"->PublicKeyHash.build->Result.getExn,
  alias: "Token2",
  symbol: "TK2",
  chain: "NetXz969SFaFn8k",
  decimals: 10,
  asset:
    Metadata.{
      ...defaultAsset,
      blocklevel: Some(12934),
      isBooleanAmount: false,
      description: Some("Undestructible NFT"),
      isTransferable: false,
      formats: None,
      attributes:
        Some([|{name: "kind", value: "non_transferable", type_: None}|]),
    },
};

let assetJson = {|
  {
    "blocklevel": 12934,
    "isBooleanAmount": false,
    "description": "Undestructible NFT",
    "isTransferable": false,
    "formats": null,
    "attributes": [{"name": "kind", "value":"non_transferable"}]
  }
|};

let token1Json = {|
   {
     "kind": "fa1-2",
     "address": "KT1F8BQQcAbs9LthTMsFMAHs2L2QN5wnaFgL",
     "alias": "Token1",
     "symbol": "TK1",
     "chain": "NetXz969SFaFn8k",
     "decimals": 2,
     "asset": {}
   }
|};

let token2Json =
  Format.sprintf(
    {|
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
|},
    assetJson,
  );

describe("Tokens", ({test}) => {
  test("Parse FA1.2", ({expectEqual}) => {
    let token =
      token1Json
      ->JsonEx.parse
      ->Result.flatMap(json => json->JsonEx.decode(Token.Decode.record));
    expectEqual(token, Ok(token1));
  });

  test("Parse FA2", ({expectEqual}) => {
    let token =
      token2Json
      ->JsonEx.parse
      ->Result.flatMap(json => json->JsonEx.decode(Token.Decode.record));
    expectEqual(token, Ok(token2));
  });

  test("Encode FA1.2", ({expectEqual}) => {
    let token1Encoded = token1->Token.Encode.record;
    let token1' = token1Encoded->JsonEx.decode(Token.Decode.record);
    expectEqual(token1', Ok(token1));
  });

  test("Encode FA2", ({expectEqual}) => {
    let token2Encoded = token2->Token.Encode.record;
    let token2' = token2Encoded->JsonEx.decode(Token.Decode.record);
    expectEqual(token2', Ok(token2));
  });
});
