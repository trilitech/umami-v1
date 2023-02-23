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
      operations: array<operation>,
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
    //   {prim: 'DROP'},
    //   {prim: 'NIL', args: [{ prim: 'operation' }]},
    // 0 {prim: 'PUSH', args: [{ prim: 'key_hash' }, { string: key }]},
    // 1 {prim: 'SOME'},
    // 2 {prim: 'SET_DELEGATE'},
    //   {prim: 'CONS'},
    // ]
    let setDelegate = (array: array<Js.Json.t>, start) => {
      let check = (pos, value) => check(array, pos, value)
      check(start + 1, {"prim": "SOME"}) && check(start + 2, {"prim": "SET_DELEGATE"})
        ? array[start]
          ->Option.flatMap(recipient("key_hash", ReTaquitoUtils.encodeKeyHash))
          ->Option.map(res => (res, start + 3))
        : None
    }

    // See MANAGER_LAMBDA.removeDelegate
    // [
    //   { prim: 'DROP' },
    //   { prim: 'NIL', args: [{ prim: 'operation' }] },
    // 0 { prim: 'NONE', args: [{ prim: 'key_hash' }] },
    // 1 { prim: 'SET_DELEGATE' },
    //   { prim: 'CONS' },
    // ]
    let removeDelegate = (array: array<Js.Json.t>, start) => {
      let check = (pos, value) => check(array, pos, value)
      check(start, {"prim": "NONE", "args": [{"prim": "key_hash"}]}) &&
      check(start + 1, {"prim": "SET_DELEGATE"})
        ? Some(start + 2)
        : None
    }

    // See MANAGER_LAMBDA.transferImplicit
    // [
    //   {prim: "DROP"},
    //   {prim: "NIL", args: [{prim: "operation"}]},
    // 0 {prim: "PUSH", args: [{prim: "key_hash"}, {string: <RECIPIENT>}]},
    // 1 {prim: "IMPLICIT_ACCOUNT"},
    // 2 {prim: "PUSH", args: [{prim: "mutez"}, {int: <AMOUNT>}]},
    // 3 {prim: "UNIT"},
    // 4 {prim: "TRANSFER_TOKENS"},
    //   {prim: "CONS"},
    // ]
    let transferImplicit = (array: array<Js.Json.t>, start) => {
      let check = (pos, value) => check(array, pos, value)
      check(start + 1, {"prim": "IMPLICIT_ACCOUNT"}) &&
      check(start + 3, {"prim": "UNIT"}) &&
      check(start + 4, {"prim": "TRANSFER_TOKENS"})
        ? recipient_amount(
            start,
            "key_hash",
            ReTaquitoUtils.encodeKeyHash,
            start + 2,
            array,
          )->Option.map(res => (res, start + 5))
        : None
    }

    // See MANAGER_LAMBDA.transferToContract
    // [
    //   { prim: 'DROP' },
    //   { prim: 'NIL', args: [{ prim: 'operation' }] },
    // 0 { prim: 'PUSH', args: [{ prim: 'address' }, { string: key }], },
    // 1 { prim: 'CONTRACT', args: [{ prim: 'unit' }] },
    // 2 [ { prim: 'IF_NONE', args: [[[{ prim: 'UNIT' }, { prim: 'FAILWITH' }]], []], }, ],
    // 3 { prim: 'PUSH', args: [{ prim: 'mutez' }, { int: `${amount}` }], },
    // 4 { prim: 'UNIT' },
    // 5 { prim: 'TRANSFER_TOKENS' },
    //   { prim: 'CONS' },
    // ]
    let transferToContract = (array: array<Js.Json.t>, start) => {
      let check = (pos, value) => check(array, pos, value)
      check(start + 1, {"prim": "CONTRACT", "args": [{"prim": "unit"}]}) &&
      check(
        start + 2,
        [{"prim": "IF_NONE", "args": [[[{"prim": "UNIT"}, {"prim": "FAILWITH"}]], []]}],
      ) &&
      check(start + 4, {"prim": "UNIT"}) &&
      check(start + 5, {"prim": "TRANSFER_TOKENS"})
        ? recipient_amount(
            start,
            "address",
            ReTaquitoUtils.encodePubKey,
            start + 3,
            array,
          )->Option.map(res => (res, start + 6))
        : None
    }

    let transfer = (array: array<Js.Json.t>, start): option<(recipient_amount, int)> =>
      switch transferImplicit(array, start) {
      | None => transferToContract(array, start)
      | x => x
      }

    let delegate = (array: array<Js.Json.t>, start): option<(option<PublicKeyHash.t>, int)> =>
      switch setDelegate(array, start) {
      | None => removeDelegate(array, start)->Option.map(next => (None, next))
      | Some(x, next) => Some(Some(x), next)
      }

    let parseOperationsList = (json: Js.Json.t) => {
      Js.Json.decodeArray(json)->Option.flatMap(array => {
        let last = Array.length(array) - 1
        let check = (pos, value) => check(array, pos, value)
        check(0, {"prim": "DROP"}) && check(1, {"prim": "NIL", "args": [{"prim": "operation"}]})
          ? {
              let rec parse = (acc, instr) => {
                instr == 2 || check(instr, {"prim": "CONS"})
                  ? instr == last
                      ? Some(acc)
                      : {
                          let instr = instr == 2 ? 2 : instr + 1
                          open Operation
                          switch transfer(array, instr)->Option.map(res => {
                            let ({amount, recipient}, next) = res
                            (
                              Transaction.Tez({
                                amount: amount,
                                destination: recipient,
                                parameters: None,
                                entrypoint: None,
                              })->Transaction,
                              next,
                            )
                          }) {
                          | Some(x, next) => parse(Js.Array.concat(acc, [x]), next)
                          | None =>
                            delegate(array, instr)
                            ->Option.map(x => {
                              let (delegate, next) = x
                              ({Delegation.delegate: delegate}->Delegation, next)
                            })
                            ->Option.mapWithDefault(Some([Unknown]), x => {
                              let (x, next) = x
                              parse(Js.Array.concat(acc, [x]), next)
                            })
                          }
                        }
                  : None
              }
              parse([], 2)
            }
          : None
      })
    }
  }

  let parseActions = (actions): option<array<Operation.payload>> => {
    actions->Json.parse->Option.flatMap(LAMBDA_PARSER.parseOperationsList)
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
          parseActions(value.actions)->Option.map(operations => {
            PendingOperation.id: key,
            operations: operations,
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
    ProtocolHelper.Origination.make(
      ~balance=Tez.zero,
      ~code=MultisigMicheline.code,
      ~storage=MultisigMicheline.storage(source.Account.address, ownerAddresses, threshold),
      ~delegate=None,
      (),
    )
  }
}

let executionEntrypoint: ReTaquitoTypes.Transfer.Entrypoint.t = "execute"
