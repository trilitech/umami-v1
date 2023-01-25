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
  | #Ghostnet => "KT1Mqvf7bnYe4Ty2n7ZbGkdbebCd4WoTJUUp"->PublicKeyHash.build->Result.getExn
  | _ => ""->PublicKeyHash.build->Result.getExn
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
  let getAddresses = (network, ~addresses: array<PublicKeyHash.t>, ~contract: PublicKeyHash.t) => {
    let addresses = addresses->List.fromArray
    network
    ->ServerAPI.Explorer.getMultisigs(~addresses, ~contract)
    ->Promise.mapOk(response => {
      response->Array.reduce(Set.make(~id=module(PublicKeyHash.Comparator)), (contracts, (_, ks)) =>
        contracts->Set.mergeMany(ks)
      )
    })
    ->Promise.mapOk(Set.toArray)
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

  let multisigFromStorage = (network: Network.t, contract: PublicKeyHash.t, storage: Storage.t) => {
    let alias = {
      let s = (contract :> string)
      "Multisig " ++ (String.sub(s, 3, 3) ++ "..." ++ String.sub(s, String.length(s) - 3, 3))
    }
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
    ->Promise.mapOk(map =>
      map->PublicKeyHash.Map.merge(Cache.get()->Result.getWithDefault(PublicKeyHash.Map.empty), (
        _,
        updated,
        cached,
      ) => updated == None ? cached : updated)
    )
    ->Promise.tapOk(Cache.set)
  }

  module Bigmap = {
    module Entry = {
      module Value = {
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

      type t = {
        key: ReBigNumber.t,
        value: Value.t,
      }

      let decoder = json => {
        open Json.Decode
        {
          key: json |> field("key", string) |> ReBigNumber.fromString,
          value: json |> field("value", Value.decoder),
        }
      }
    }
  }

  module PendingOperation = {
    type type_ = Transaction

    type t = {
      id: ReBigNumber.t,
      type_: type_,
      amount: ReBigNumber.t,
      recipient: PublicKeyHash.t,
      approvals: array<PublicKeyHash.t>,
    }
  }

  module Statement = {
    type t = {
      prim: string,
      args: option<array<Js.Dict.t<string>>>,
    }

    let decoder = json => {
      open Json.Decode
      {
        prim: json |> field("prim", string),
        args: json |> optional(field("args", array(dict(string)))),
      }
    }
  }

  let parseActions = actions => {
    actions
    ->JsonEx.parse
    ->Result.flatMap(json => json->JsonEx.decode(Json.Decode.array(Statement.decoder)))
    ->Result.flatMap(statements => {
      let amount =
        statements[4]
        ->Option.flatMap(s => s.args)
        ->Option.flatMap(args => args[1])
        ->Option.flatMap(arg => arg->Js.Dict.get("int"))
        ->Option.map(ReBigNumber.fromString)
      let recipient =
        statements[2]
        ->Option.flatMap(s => s.args)
        ->Option.flatMap(args => args[1])
        ->Option.flatMap(arg =>
          arg
          ->Js.Dict.get("bytes")
          ->Option.map(s => s->ReTaquitoUtils.encodeKeyHash->PublicKeyHash.build->Result.getExn)
        )
      switch (amount, recipient) {
      | (Some(amount), Some(recipient)) => Some((amount, recipient))
      | _ => None
      }->ResultEx.fromOption(JsonEx.ParsingError(""))
    })
  }

  let getPendingOperations = (network: Network.t, ~bigmap: int) =>
    ServerAPI.URL.External.tzktBigmapKeys(~network, ~bigmap)
    ->Promise.value
    ->Promise.flatMapOk(url => url->ServerAPI.URL.get)
    ->Promise.flatMapOk(json =>
      json->JsonEx.decode(Json.Decode.array(Bigmap.Entry.decoder))->Promise.value
    )
    ->Promise.flatMapOk(entries =>
      entries
      ->Array.map(entry => {
        parseActions(entry.value.actions)
        ->Result.map(((amount, recipient)) => {
          PendingOperation.id: entry.key,
          type_: Transaction,
          amount: amount,
          recipient: recipient,
          approvals: entry.value.approvals,
        })
        ->Promise.value
      })
      ->Promise.allArray
    )
    ->Promise.mapOk(results =>
      results->Array.reduce(ReBigNumber.Map.empty, (map, result) =>
        switch result {
        | Ok(pendingOperation) => map->ReBigNumber.Map.set(pendingOperation.id, pendingOperation)
        | _ => map
        }
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
