type t = {
  address: PublicKeyHash.t,
  alias: string,
  balance: ReBigNumber.t,
  chain: Network.chainId,
  signers: array<PublicKeyHash.t>,
  threshold: ReBigNumber.t,
}

let contract = chain =>
  switch chain {
  | #Mainnet => "KT1Lw11GPDxpdWXWudFUUBMA2Cihevmt8QCf"->PublicKeyHash.build->Result.getExn->Some
  | #Ghostnet => "KT1Mqvf7bnYe4Ty2n7ZbGkdbebCd4WoTJUUp"->PublicKeyHash.build->Result.getExn->Some
  | #Limanet => "KT1W7ZJqgwj5D3aDwDSwZiSSvzKTaZNbiVLC"->PublicKeyHash.build->Result.getExn->Some
  | #Custom(_)
  | #Edo2net
  | #Florencenet
  | #Granadanet
  | #Hangzhounet
  | #Jakartanet
  | #Kathmandunet
  | #Mumbainet =>
    None
  }

module Cache = {
  module JSON = {
    let decoder = json => {
      open Json.Decode
      {
        address: json |> field("address", string) |> PublicKeyHash.build |> Result.getExn,
        alias: json |> field("alias", string),
        balance: json |> field("balance", string) |> ReBigNumber.fromString,
        chain: json |> field("chain", Network.Decode.chainIdDecoder),
        signers: json |> field("signers", array(PublicKeyHash.decoder)),
        threshold: json |> field("threshold", string) |> ReBigNumber.fromString,
      }
    }

    let encoder = t => {
      open Json.Encode
      object_(list{
        ("address", (t.address :> string) |> string),
        ("alias", t.alias |> string),
        ("balance", t.balance |> ReBigNumber.toString |> string),
        ("chain", t.chain |> Network.Encode.chainIdEncoder),
        ("signers", t.signers |> array(PublicKeyHash.encoder)),
        ("threshold", t.threshold |> ReBigNumber.toString |> string),
      })
    }
  }

  include LocalStorage.Make({
    let key = "registered-multisigs"

    type t = PublicKeyHash.Map.map<t>

    let decoder = json => {
      open PublicKeyHash.Map
      (json |> Json.Decode.array(JSON.decoder))
        ->Array.reduce(empty, (map, contract) => map->set(contract.address, contract))
    }

    let encoder = t => {
      t->PublicKeyHash.Map.valuesToArray |> Json.Encode.array(JSON.encoder)
    }
  })

  let getWithFallback = () =>
    switch get() {
    | Error(LocalStorage.NotFound(_)) => Ok(PublicKeyHash.Map.empty)
    | map => map
    }
}

module API = {
  // Get contract addresses of contracts that
  // - contain provided addresses param
  // - have the same code as contract param
  let getAddresses = (network, ~addresses: array<PublicKeyHash.t>) => {
    switch contract(network.Network.chain) {
    | Some(contract) =>
      let addresses = addresses->List.fromArray
      network
      ->ServerAPI.Explorer.getMultisigs(~addresses, ~contract)
      ->Promise.mapOk(response => {
        response->Array.reduce(Set.make(~id=module(PublicKeyHash.Comparator)), (
          contracts,
          (_, ks),
        ) => contracts->Set.mergeMany(ks))
      })
      ->Promise.mapOk(Set.toArray)
    | None => Promise.ok([])
    }
  }

  module Storage = {
    type t = {
      lastOpID: ReBigNumber.t,
      metadata: int,
      owner: PublicKeyHash.t,
      pendingOps: int,
      signers: array<PublicKeyHash.t>,
      threshold: ReBigNumber.t,
    }

    let decoder = json => {
      open Json.Decode
      {
        lastOpID: json |> field("last_op_id", string) |> ReBigNumber.fromString,
        metadata: json |> field("metadata", int),
        owner: json |> field("owner", PublicKeyHash.decoder),
        pendingOps: json |> field("pending_ops", int),
        signers: json |> field("signers", array(PublicKeyHash.decoder)),
        threshold: json |> field("threshold", string) |> ReBigNumber.fromString,
      }
    }
  }

  let getStorage = (network: Network.t, ~contract: PublicKeyHash.t) => {
    ServerAPI.URL.External.tzktContractStorage(~network, ~contract)
    ->Promise.value
    ->Promise.flatMapOk(url => url->ServerAPI.URL.get)
    ->Promise.flatMapOk(json => json->JsonEx.decode(Storage.decoder)->Promise.value)
  }

  let getAllFromCache = (network: Network.t) =>
    Cache.getWithFallback()->Result.map(map =>
      map->PublicKeyHash.Map.keep((_, v) => v.chain == network.chain->Network.getChainId)
    )

  let removeFromCache = (contracts: array<PublicKeyHash.t>) => {
    Cache.getWithFallback()->Result.map(cache =>
      Array.reduce(contracts, cache, (cache, contract) =>
        PublicKeyHash.Map.remove(cache, contract)
      )->Cache.set
    )
  }

  // Invariant: defaultName called twice must return the same name for the same KT1
  let defaultName = (contract: PublicKeyHash.t) => {
    let s = (contract :> string)
    "Multisig " ++ (String.sub(s, 3, 3) ++ "..." ++ String.sub(s, String.length(s) - 3, 3))
  }

  let multisigFromStorage = (network: Network.t, contract: PublicKeyHash.t, storage: Storage.t) => {
    let alias = defaultName(contract)
    {
      address: contract,
      alias: alias,
      balance: ReBigNumber.zero,
      chain: network.chain->Network.getChainId,
      signers: storage.signers,
      threshold: storage.threshold,
    }
  }

  let getOneFromNetwork = (network: Network.network, contract: PublicKeyHash.t) => {
    getStorage(network, ~contract)->Promise.mapOk(storage =>
      multisigFromStorage(network, contract, storage)
    )
  }

  let get = (network: Network.t, ~forceRefresh=false, contracts: array<PublicKeyHash.t>) => {
    let cache = getAllFromCache(network)->Result.getWithDefault(PublicKeyHash.Map.empty)
    let res =
      Array.map(contracts, contract => {
        switch PublicKeyHash.Map.get(cache, contract) {
        | Some(v) =>
          forceRefresh
            ? network
              ->getStorage(~contract)
              ->Promise.mapOk(s => (
                contract,
                #multisig({...v, signers: s.signers, threshold: s.threshold}),
              ))
            : Promise.ok((contract, #multisig(v)))
        | None =>
          network->getStorage(~contract)->Promise.mapOk(storage => (contract, #storage(storage)))
        }
      })
      ->Promise.allArray
      ->Promise.mapOk(responses =>
        responses->Array.reduce(PublicKeyHash.Map.empty, (map, response) =>
          switch response {
          | Ok((contract, #multisig(m))) => map->PublicKeyHash.Map.set(contract, m)
          | Ok((contract, #storage(s))) =>
            let multisig = multisigFromStorage(network, contract, s)
            map->PublicKeyHash.Map.set(contract, multisig)
          | _ => map
          }
        )
      )
    let merge = (update, cache) =>
      PublicKeyHash.Map.merge(update, cache, (_, updated, cached) =>
        updated == None ? cached : updated
      )

    let res = res->Promise.mapOk(res => merge(res, cache))
    let () =
      res
      ->Promise.mapOk(res =>
        merge(res, Cache.get()->Result.getWithDefault(PublicKeyHash.Map.empty))
      )
      ->Promise.tapOk(Cache.set)
      ->Promise.ign

    res
  }

  module Bigmap = {
    module ParamKey = {
      type t = ReBigNumber.t
      let decoder = json => json |> Json.Decode.string |> ReBigNumber.fromString
    }
    module ParamValue = {
      type t = {
        actions: string,
        approvals: array<PublicKeyHash.t>,
      }
      let decoder = json => {
        open Json.Decode
        {
          actions: json |> field("actions", string),
          approvals: json |> field("approvals", array(PublicKeyHash.decoder)),
        }
      }
    }
    include Tzkt.Bigmap(ParamKey, ParamValue)
  }

  module PendingOperation = {
    type operation = Operation.payload

    type t = {
      id: ReBigNumber.t,
      operation: operation,
      approvals: array<PublicKeyHash.t>,
      raw: string,
    }
  }

  module LAMBDA_PARSER = {
    let check = (array, pos, value) =>
      array[pos]->Option.mapWithDefault(false, x => x == Obj.magic(value))

    // For a given <FIELD> value:
    // read {prim: "PUSH", args: [{prim: "<FIELD>"}, {bytes: <RECIPIENT>}]} and
    // return <RECIPIENT>
    let recipient = (field, encode, json) => {
      Js.Json.decodeObject(json)
      ->Option.flatMap(x =>
        Js_dict.get(x, "prim") == Some(Obj.magic("PUSH")) ? Js_dict.get(x, "args") : None
      )
      ->Option.flatMap(Js.Json.decodeArray)
      ->Option.flatMap(x => x[0] == Some(Obj.magic({"prim": field})) ? x[1] : None)
      ->Option.flatMap(Js.Json.decodeObject)
      ->Option.flatMap(x => Js.Dict.get(x, "bytes")) // is "string" in LAMBDA_MANAGER
      ->Option.flatMap(Js.Json.decodeString)
      ->Option.map(encode)
      ->Option.map(PublicKeyHash.build)
      ->Option.map(Result.getExn)
    }

    // Extract <AMOUNT> from {prim: "PUSH", args: [{prim: "mutez"}, {int: <AMOUNT>}]}
    let amount = json =>
      Js.Json.decodeObject(json)
      ->Option.flatMap(x =>
        Js_dict.get(x, "prim") == Some(Obj.magic("PUSH")) ? Js_dict.get(x, "args") : None
      )
      ->Option.flatMap(Js.Json.decodeArray)
      ->Option.flatMap(x => x[0] == Some(Obj.magic({"prim": "mutez"})) ? x[1] : None)
      ->Option.flatMap(Js.Json.decodeObject)
      ->Option.flatMap(x => Js.Dict.get(x, "int"))
      ->Option.flatMap(Js.Json.decodeString)
      ->Option.map(Tez.fromMutezString)

    type recipient_amount = {
      recipient: PublicKeyHash.t,
      amount: Tez.t,
    }

    let recipient_amount = (r, rk, encode, a, array): option<recipient_amount> => {
      array[r]
      ->Option.flatMap(recipient(rk, encode))
      ->Option.flatMap(recipient => {
        array[a]
        ->Option.flatMap(amount)
        ->Option.map(amount => {
          {recipient: recipient, amount: amount}
        })
      })
    }

    // See MANAGER_LAMBDA.setDelegate
    // [
    // 0 {prim: 'DROP'},
    // 1 {prim: 'NIL', args: [{ prim: 'operation' }]},
    // 2 {prim: 'PUSH', args: [{ prim: 'key_hash' }, { string: key }]},
    // 3 {prim: 'SOME'},
    // 4 {prim: 'SET_DELEGATE'},
    // 5 {prim: 'CONS'},
    // ]
    let setDelegate = (json: Js.Json.t) => {
      Js.Json.decodeArray(json)->Option.flatMap(array => {
        let check = (pos, value) => check(array, pos, value)
        Array.length(array) == 6 &&
        check(0, {"prim": "DROP"}) &&
        check(1, {"prim": "NIL", "args": [{"prim": "operation"}]}) &&
        check(3, {"prim": "SOME"}) &&
        check(4, {"prim": "SET_DELEGATE"}) &&
        check(5, {"prim": "CONS"})
          ? array[2]->Option.flatMap(recipient("key_hash", ReTaquitoUtils.encodeKeyHash))
          : None
      })
    }

    // See MANAGER_LAMBDA.removeDelegate
    // [
    // 0 { prim: 'DROP' },
    // 1 { prim: 'NIL', args: [{ prim: 'operation' }] },
    // 2 { prim: 'NONE', args: [{ prim: 'key_hash' }] },
    // 3 { prim: 'SET_DELEGATE' },
    // 4 { prim: 'CONS' },
    // ]
    let removeDelegate = (json: Js.Json.t) => {
      Js.Json.decodeArray(json)->Option.flatMap(array => {
        let check = (pos, value) => check(array, pos, value)
        Array.length(array) == 5 &&
        check(0, {"prim": "DROP"}) &&
        check(1, {"prim": "NIL", "args": [{"prim": "operation"}]}) &&
        check(2, {"prim": "NONE", "args": [{"prim": "key_hash"}]}) &&
        check(3, {"prim": "SET_DELEGATE"}) &&
        check(4, {"prim": "CONS"})
          ? Some()
          : None
      })
    }

    // See MANAGER_LAMBDA.transferImplicit
    // [
    // 0 {prim: "DROP"},
    // 1 {prim: "NIL", args: [{prim: "operation"}]},
    // 2 {prim: "PUSH", args: [{prim: "key_hash"}, {string: <RECIPIENT>}]},
    // 3 {prim: "IMPLICIT_ACCOUNT"},
    // 4 {prim: "PUSH", args: [{prim: "mutez"}, {int: <AMOUNT>}]},
    // 5 {prim: "UNIT"},
    // 6 {prim: "TRANSFER_TOKENS"},
    // 7 {prim: "CONS"},
    // ]
    let transferImplicit = (json: Js.Json.t) => {
      Js.Json.decodeArray(json)->Option.flatMap(array => {
        let check = (pos, value) => check(array, pos, value)
        Array.length(array) == 8 &&
        check(0, {"prim": "DROP"}) &&
        check(1, {"prim": "NIL", "args": [{"prim": "operation"}]}) &&
        check(3, {"prim": "IMPLICIT_ACCOUNT"}) &&
        check(5, {"prim": "UNIT"}) &&
        check(6, {"prim": "TRANSFER_TOKENS"}) &&
        check(7, {"prim": "CONS"})
          ? recipient_amount(2, "key_hash", ReTaquitoUtils.encodeKeyHash, 4, array)
          : None
      })
    }

    // See MANAGER_LAMBDA.transferToContract
    // [
    // 0 { prim: 'DROP' },
    // 1 { prim: 'NIL', args: [{ prim: 'operation' }] },
    // 2 { prim: 'PUSH', args: [{ prim: 'address' }, { string: key }], },
    // 3 { prim: 'CONTRACT', args: [{ prim: 'unit' }] },
    // 4 [ { prim: 'IF_NONE', args: [[[{ prim: 'UNIT' }, { prim: 'FAILWITH' }]], []], }, ],
    // 5 { prim: 'PUSH', args: [{ prim: 'mutez' }, { int: `${amount}` }], },
    // 6 { prim: 'UNIT' },
    // 7 { prim: 'TRANSFER_TOKENS' },
    // 8 { prim: 'CONS' },
    // ]
    let transferToContract = (json: Js.Json.t) => {
      Js.Json.decodeArray(json)->Option.flatMap(array => {
        let check = (pos, value) => check(array, pos, value)
        Array.length(array) == 9 &&
        check(0, {"prim": "DROP"}) &&
        check(1, {"prim": "NIL", "args": [{"prim": "operation"}]}) &&
        check(3, {"prim": "CONTRACT", "args": [{"prim": "unit"}]}) &&
        check(4, [{"prim": "IF_NONE", "args": [[[{"prim": "UNIT"}, {"prim": "FAILWITH"}]], []]}]) &&
        check(6, {"prim": "UNIT"}) &&
        check(7, {"prim": "TRANSFER_TOKENS"}) &&
        check(8, {"prim": "CONS"})
          ? recipient_amount(2, "address", ReTaquitoUtils.encodePubKey, 5, array)
          : None
      })
    }

    let transfer = (json: Js.Json.t): option<recipient_amount> =>
      switch transferImplicit(json) {
      | None => transferToContract(json)
      | x => x
      }

    let delegate = (json: Js.Json.t): option<option<PublicKeyHash.t>> =>
      switch setDelegate(json) {
      | None => removeDelegate(json)->Option.map(() => None)
      | x => Some(x)
      }
  }

  let parseActions = (actions): option<Operation.payload> => {
    actions
    ->Json.parse
    ->Option.flatMap(x => {
      open Operation
      switch LAMBDA_PARSER.transfer(x)->Option.map(({amount, recipient}) =>
        Transaction.Tez({
          amount: amount,
          destination: recipient,
          parameters: None,
          entrypoint: None,
        })->Transaction
      ) {
      | None =>
        LAMBDA_PARSER.delegate(x)->Option.map(delegate =>
          {Delegation.delegate: delegate}->Delegation
        )
      | x => x
      }
    })
  }

  let getPendingOperations = (network: Network.t, ~bigmap: int) =>
    ServerAPI.URL.External.tzktBigmapKeys(~network, ~bigmap)
    ->Promise.value
    ->Promise.flatMapOk(url => url->ServerAPI.URL.get)
    ->Promise.flatMapOk(json =>
      json->JsonEx.decode(Json.Decode.array(Bigmap.Key.decoder))->Promise.value
    )
    ->Promise.mapOk(entries =>
      entries->Array.keepMap(entry => {
        switch (entry.active, entry.key, entry.value) {
        | (true, Some(key), Some(value)) =>
          parseActions(value.actions)->Option.map(payload => {
            PendingOperation.id: key,
            operation: payload,
            approvals: value.approvals,
            raw: value.actions,
          })
        | _ => None
        }
      })
    )
    ->Promise.mapOk(results =>
      results->Array.reduce(ReBigNumber.Map.empty, (map, pendingOperation) =>
        map->ReBigNumber.Map.set(pendingOperation.PendingOperation.id, pendingOperation)
      )
    )

  @ocaml.doc(
    "Generate the operation to be used for simulation/signed/sent in order to create (originate) a new multisig contract"
  )
  let origination = (~source, ~ownerAddresses, ~threshold) => {
    open Protocol
    {
      source: source,
      managers: [
        ProtocolHelper.Origination.make(
          ~balance=Tez.zero,
          ~code=MultisigMicheline.code,
          ~storage=MultisigMicheline.storage(source.address, ownerAddresses, threshold),
          ~delegate=None,
          (),
        ),
      ],
    }
  }
}
