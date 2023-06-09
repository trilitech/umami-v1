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
  | #Mumbainet => "KT1FqCVKxbof2wRPxfLSRnMJfKH39Zh912AK"->PublicKeyHash.build->Result.getExn->Some
  | #Custom(_)
  | #Edo2net
  | #Florencenet
  | #Granadanet
  | #Hangzhounet
  | #Jakartanet
  | #Kathmandunet =>
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
      ServerAPI.Explorer.Tzkt.getMultisigs(network, ~addresses, ~contract)
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
    ->Promise.mapError(e =>
      I18n.Errors.fetching_storage((contract :> string), e->Errors.toString)->Errors.Generic
    )
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

  let parseActions = (actions): option<array<Operation.payload>> => {
    actions->Json.parse->Option.flatMap(Michelson.LAMBDA_PARSER.parseOperationsList)
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

module Entrypoint = {
  let propose: ReTaquitoTypes.Transfer.Entrypoint.t = "propose"
  let approve: ReTaquitoTypes.Transfer.Entrypoint.t = "approve"
  let execute: ReTaquitoTypes.Transfer.Entrypoint.t = "execute"
}
