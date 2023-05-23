open TestFramework

let config = {
  open ConfigContext
  {
    defaultNetwork: true,
    network: Network.ghostnet,
    customNetworks: [],
    theme: #system,
    baseDir: () => {
      open System
      open Path.Ops
      appDir() / !"tezos-client"
    },
    confirmations: 5,
    backupFile: None,
    autoUpdates: true,
  }
}

let pkh = s => s->PublicKeyHash.build->Result.getExn

describe("API tests", ({testAsync}) => {
  testAsync("runs valid balance test", ({expect, callback}) => {
    module Stub = {
      let _ = (_, ~inputs=?, ()) => {
        ignore(inputs)
        Promise.value(Ok("0.00"))
      }
    }
    module UnderTest = NodeAPI.Balance
    UnderTest.get(config, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"->pkh, ())->Promise.get(result => {
      expect.value(result).toEqual(Result.Ok(Tez.zero))
      callback()
    })
    ()
  })

  testAsync("runs invalid account.delegates test", ({expect, callback}) => {
    module Stub = {
      let get = _ => {
        let data = `[
          {
            "name": "foo",
            "address": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"
          },
          {
            "address": "tz1NF7b38uQ43N4nmTHvDKpr1Qo5LF9iYawk"
          }
        ]`
        Promise.value(Ok(data->Json.parseOrRaise))
      }
    }
    module UnderTest = NodeAPI.DelegateMaker(Stub)
    UnderTest.getBakers(config)->Promise.get(result => {
      expect.value(result).toEqual(
        Result.Error("Expected field 'name'\n\tin array at index 1"->Errors.Generic),
      )
      callback()
    })
    ()
  })
})
