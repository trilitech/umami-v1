open ReTaquitoTypes

type t = {
  address: PublicKeyHash.t,
  alias: string,
  balance: ReBigNumber.t,
  chain: Network.chainId,
  signers: array<PublicKeyHash.t>,
  threshold: int,
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
        threshold: json |> field("threshold", int),
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
        ("threshold", t.threshold |> int),
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
  let getAddresses = (network, ~addresses: array<PublicKeyHash.t>, ~contract: PublicKeyHash.t) => {
    let addresses = addresses->List.fromArray
    Js.log(__LOC__)
    Js.log(addresses)
    Js.log(__LOC__)
    network
    ->ServerAPI.Explorer.getMultisigs(~addresses, ~contract)
    ->Promise.mapOk(response => {
      Js.log(__LOC__)
      Js.log(response)
      Js.log(__LOC__)
      response->Array.reduce(Set.make(~id=module(PublicKeyHash.Comparator)), (contracts, (_, ks)) =>
        contracts->Set.mergeMany(ks)
      )
    })
    ->Promise.mapOk(Set.toArray)
  }

  module Storage = {
    type t = {
      lastOpID: int,
      metadata: int,
      owner: PublicKeyHash.t,
      pendingOps: int,
      signers: array<PublicKeyHash.t>,
      threshold: int,
    }

    let decoder = json => {
      open Json.Decode
      {
        lastOpID: (json |> field("last_op_id", string))->Int.fromString->Option.getWithDefault(0),
        metadata: json |> field("metadata", int),
        owner: json |> field("owner", PublicKeyHash.decoder),
        pendingOps: json |> field("pending_ops", int),
        signers: json |> field("signers", array(PublicKeyHash.decoder)),
        threshold: (json |> field("threshold", string))->Int.fromString->Option.getWithDefault(0),
      }
    }
  }

  let getStorage = (network, ~contract: PublicKeyHash.t) => {
    ServerAPI.URL.External.tzktContractStorage(~network, ~contract)
    ->Promise.value
    ->Promise.flatMapOk(url => url->ServerAPI.URL.get)
    ->Promise.flatMapOk(json => json->JsonEx.decode(Storage.decoder)->Promise.value)
  }

  let getFromCache = (network: Network.t) =>
    Cache.getWithFallback()->Result.map(map =>
      map->PublicKeyHash.Map.keep((_, v) => v.chain == network.chain->Network.getChainId)
    )

  let get = (network: Network.t, ~addresses: array<PublicKeyHash.t>, ~contract: PublicKeyHash.t) =>
    network
    ->getAddresses(~addresses, ~contract)
    // fetch storage for each multisig found on chain
    ->Promise.flatMapOk(contracts =>
      contracts
      ->Array.map(contract =>
        network->getStorage(~contract)->Promise.mapOk(storage => (contract, storage))
      )
      ->Promise.allArray
      ->Promise.mapOk(responses =>
        responses->Array.reduce(PublicKeyHash.Map.empty, (map, response) =>
          switch response {
          | Ok((contract, storage)) => map->PublicKeyHash.Map.set(contract, storage)
          | _ => map
          }
        )
      )
    )
    // update cache
    ->Promise.mapOk(map =>
      map->PublicKeyHash.Map.merge(Cache.get()->Result.getWithDefault(PublicKeyHash.Map.empty), (
        contract,
        fromNetwork,
        fromCache,
      ) =>
        switch (fromNetwork, fromCache) {
        | (Some(storage), Some(multisig)) =>
          Some({...multisig, signers: storage.signers, threshold: storage.threshold}) // update cached multisig
        | (Some(storage), _) =>
          // cache new discovered multisig
          Some({
            address: contract,
            alias: "Multisig #" ++ Js.Date.now()->Float.toInt->Int.toString,
            balance: ReBigNumber.zero,
            chain: network.chain->Network.getChainId,
            signers: storage.signers,
            threshold: storage.threshold,
          })
        | (_, Some(multisig)) => Some(multisig) // cached multisig not found on chain, should not happen
        | _ => None
        }
      )
    )
    ->Promise.tapOk(Js.log)
    ->Promise.tapOk(cache => Cache.set(cache))

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
        key: int,
        value: Value.t,
      }

      let decoder = json => {
        open Json.Decode
        {
          key: (json |> field("key", string))->Int.fromString->Option.getWithDefault(0),
          value: json |> field("value", Value.decoder),
        }
      }
    }
  }

  module PendingOperation = {
    type type_ = Transaction

    type t = {
      id: int,
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
      results->Array.reduce(Map.Int.empty, (map, result) =>
        switch result {
        | Ok(pendingOperation) => map->Map.Int.set(pendingOperation.id, pendingOperation)
        | _ => map
        }
      )
    )
}

let code: Code.t = %raw(`[
  {
    "prim": "storage",
    "args": [
      {
        "prim": "pair",
        "args": [
          {
            "prim": "address",
            "annots": [
              "%owner"
            ]
          },
          {
            "prim": "pair",
            "args": [
              {
                "prim": "set",
                "args": [
                  {
                    "prim": "address"
                  }
                ],
                "annots": [
                  "%signers"
                ]
              },
              {
                "prim": "pair",
                "args": [
                  {
                    "prim": "nat",
                    "annots": [
                      "%threshold"
                    ]
                  },
                  {
                    "prim": "pair",
                    "args": [
                      {
                        "prim": "nat",
                        "annots": [
                          "%last_op_id"
                        ]
                      },
                      {
                        "prim": "pair",
                        "args": [
                          {
                            "prim": "big_map",
                            "args": [
                              {
                                "prim": "nat"
                              },
                              {
                                "prim": "pair",
                                "args": [
                                  {
                                    "prim": "lambda",
                                    "args": [
                                      {
                                        "prim": "unit"
                                      },
                                      {
                                        "prim": "list",
                                        "args": [
                                          {
                                            "prim": "operation"
                                          }
                                        ]
                                      }
                                    ],
                                    "annots": [
                                      "%actions"
                                    ]
                                  },
                                  {
                                    "prim": "set",
                                    "args": [
                                      {
                                        "prim": "address"
                                      }
                                    ],
                                    "annots": [
                                      "%approvals"
                                    ]
                                  }
                                ]
                              }
                            ],
                            "annots": [
                              "%pending_ops"
                            ]
                          },
                          {
                            "prim": "big_map",
                            "args": [
                              {
                                "prim": "string"
                              },
                              {
                                "prim": "bytes"
                              }
                            ],
                            "annots": [
                              "%metadata"
                            ]
                          }
                        ]
                      }
                    ]
                  }
                ]
              }
            ]
          }
        ]
      }
    ]
  },
  {
    "prim": "parameter",
    "args": [
      {
        "prim": "or",
        "args": [
          {
            "prim": "or",
            "args": [
              {
                "prim": "or",
                "args": [
                  {
                    "prim": "bytes",
                    "annots": [
                      "%set_metadata_uri"
                    ]
                  },
                  {
                    "prim": "unit",
                    "annots": [
                      "%default"
                    ]
                  }
                ]
              },
              {
                "prim": "or",
                "args": [
                  {
                    "prim": "lambda",
                    "args": [
                      {
                        "prim": "unit"
                      },
                      {
                        "prim": "list",
                        "args": [
                          {
                            "prim": "operation"
                          }
                        ]
                      }
                    ],
                    "annots": [
                      "%propose"
                    ]
                  },
                  {
                    "prim": "nat",
                    "annots": [
                      "%approve"
                    ]
                  }
                ]
              }
            ]
          },
          {
            "prim": "nat",
            "annots": [
              "%execute"
            ]
          }
        ]
      }
    ]
  },
  {
    "prim": "code",
    "args": [
      [
        {
          "prim": "NIL",
          "args": [
            {
              "prim": "operation"
            }
          ]
        },
        {
          "prim": "DIG",
          "args": [
            {
              "int": "1"
            }
          ]
        },
        {
          "prim": "UNPAIR"
        },
        {
          "prim": "DIP",
          "args": [
            {
              "int": "1"
            },
            [
              {
                "prim": "UNPAIR",
                "args": [
                  {
                    "int": "6"
                  }
                ]
              }
            ]
          ]
        },
        {
          "prim": "IF_LEFT",
          "args": [
            [
              {
                "prim": "IF_LEFT",
                "args": [
                  [
                    {
                      "prim": "IF_LEFT",
                      "args": [
                        [
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "2"
                              }
                            ]
                          },
                          {
                            "prim": "SENDER"
                          },
                          {
                            "prim": "COMPARE"
                          },
                          {
                            "prim": "EQ"
                          },
                          {
                            "prim": "NOT"
                          },
                          {
                            "prim": "IF",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "INVALID_CALLER"
                                    }
                                  ]
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "7"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "2"
                              }
                            ]
                          },
                          {
                            "prim": "SOME"
                          },
                          {
                            "prim": "PUSH",
                            "args": [
                              {
                                "prim": "string"
                              },
                              {
                                "string": ""
                              }
                            ]
                          },
                          {
                            "prim": "UPDATE"
                          },
                          {
                            "prim": "DIP",
                            "args": [
                              {
                                "int": "1"
                              },
                              [
                                {
                                  "prim": "DIG",
                                  "args": [
                                    {
                                      "int": "6"
                                    }
                                  ]
                                },
                                {
                                  "prim": "DROP",
                                  "args": [
                                    {
                                      "int": "1"
                                    }
                                  ]
                                }
                              ]
                            ]
                          },
                          {
                            "prim": "DUG",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "DROP",
                            "args": [
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "PAIR",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "DIG",
                            "args": [
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "PAIR"
                          }
                        ],
                        [
                          {
                            "prim": "DROP",
                            "args": [
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "3"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "3"
                              }
                            ]
                          },
                          {
                            "prim": "SIZE"
                          },
                          {
                            "prim": "COMPARE"
                          },
                          {
                            "prim": "GE"
                          },
                          {
                            "prim": "NOT"
                          },
                          {
                            "prim": "IF",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "FUNDING BLOCKED SINCE PARAMS ARE INVALID"
                                    }
                                  ]
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "PAIR",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "DIG",
                            "args": [
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "PAIR"
                          }
                        ]
                      ]
                    }
                  ],
                  [
                    {
                      "prim": "IF_LEFT",
                      "args": [
                        [
                          {
                            "prim": "PUSH",
                            "args": [
                              {
                                "prim": "mutez"
                              },
                              {
                                "int": "0"
                              }
                            ]
                          },
                          {
                            "prim": "AMOUNT"
                          },
                          {
                            "prim": "COMPARE"
                          },
                          {
                            "prim": "EQ"
                          },
                          {
                            "prim": "NOT"
                          },
                          {
                            "prim": "IF",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "TO FUND CONTRACT, PLEASE USE THE DEFAULT ENTRYPOINT"
                                    }
                                  ]
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "3"
                              }
                            ]
                          },
                          {
                            "prim": "SENDER"
                          },
                          {
                            "prim": "MEM"
                          },
                          {
                            "prim": "NOT"
                          },
                          {
                            "prim": "IF",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "ONLY FOR SIGNERS"
                                    }
                                  ]
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "PUSH",
                            "args": [
                              {
                                "prim": "nat"
                              },
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "ADD"
                          },
                          {
                            "prim": "DIP",
                            "args": [
                              {
                                "int": "1"
                              },
                              [
                                {
                                  "prim": "DIG",
                                  "args": [
                                    {
                                      "int": "4"
                                    }
                                  ]
                                },
                                {
                                  "prim": "DROP",
                                  "args": [
                                    {
                                      "int": "1"
                                    }
                                  ]
                                }
                              ]
                            ]
                          },
                          {
                            "prim": "DUG",
                            "args": [
                              {
                                "int": "4"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "MEM"
                          },
                          {
                            "prim": "IF",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "pending_ops"
                                    }
                                  ]
                                },
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "KEY_EXISTS"
                                    }
                                  ]
                                },
                                {
                                  "prim": "PAIR"
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              [
                                {
                                  "prim": "DUP",
                                  "args": [
                                    {
                                      "int": "6"
                                    }
                                  ]
                                },
                                {
                                  "prim": "EMPTY_SET",
                                  "args": [
                                    {
                                      "prim": "address"
                                    }
                                  ]
                                },
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "bool"
                                    },
                                    {
                                      "prim": "True"
                                    }
                                  ]
                                },
                                {
                                  "prim": "SENDER"
                                },
                                {
                                  "prim": "UPDATE"
                                },
                                {
                                  "prim": "DUP",
                                  "args": [
                                    {
                                      "int": "3"
                                    }
                                  ]
                                },
                                {
                                  "prim": "PAIR"
                                },
                                {
                                  "prim": "SOME"
                                },
                                {
                                  "prim": "DUP",
                                  "args": [
                                    {
                                      "int": "7"
                                    }
                                  ]
                                },
                                {
                                  "prim": "UPDATE"
                                },
                                {
                                  "prim": "DIP",
                                  "args": [
                                    {
                                      "int": "1"
                                    },
                                    [
                                      {
                                        "prim": "DIG",
                                        "args": [
                                          {
                                            "int": "5"
                                          }
                                        ]
                                      },
                                      {
                                        "prim": "DROP",
                                        "args": [
                                          {
                                            "int": "1"
                                          }
                                        ]
                                      }
                                    ]
                                  ]
                                },
                                {
                                  "prim": "DUG",
                                  "args": [
                                    {
                                      "int": "5"
                                    }
                                  ]
                                }
                              ]
                            ]
                          },
                          {
                            "prim": "DROP",
                            "args": [
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "PAIR",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "DIG",
                            "args": [
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "PAIR"
                          }
                        ],
                        [
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "2"
                              }
                            ]
                          },
                          {
                            "prim": "GET"
                          },
                          {
                            "prim": "IF_NONE",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "INVALID OP ID"
                                    }
                                  ]
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "PUSH",
                            "args": [
                              {
                                "prim": "mutez"
                              },
                              {
                                "int": "0"
                              }
                            ]
                          },
                          {
                            "prim": "AMOUNT"
                          },
                          {
                            "prim": "COMPARE"
                          },
                          {
                            "prim": "EQ"
                          },
                          {
                            "prim": "NOT"
                          },
                          {
                            "prim": "IF",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "TO FUND CONTRACT, PLEASE USE THE DEFAULT ENTRYPOINT"
                                    }
                                  ]
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "4"
                              }
                            ]
                          },
                          {
                            "prim": "SENDER"
                          },
                          {
                            "prim": "MEM"
                          },
                          {
                            "prim": "NOT"
                          },
                          {
                            "prim": "IF",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "ONLY FOR SIGNERS"
                                    }
                                  ]
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "DUP"
                          },
                          {
                            "prim": "CDR"
                          },
                          {
                            "prim": "SENDER"
                          },
                          {
                            "prim": "MEM"
                          },
                          {
                            "prim": "IF",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "ALREADY APPROVED"
                                    }
                                  ]
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "7"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "8"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "4"
                              }
                            ]
                          },
                          {
                            "prim": "GET"
                          },
                          {
                            "prim": "IF_NONE",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "pending_ops"
                                    }
                                  ]
                                },
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "ASSET_NOT_FOUND"
                                    }
                                  ]
                                },
                                {
                                  "prim": "PAIR"
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "UNPAIR"
                          },
                          {
                            "prim": "SWAP"
                          },
                          {
                            "prim": "DROP",
                            "args": [
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "9"
                              }
                            ]
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "5"
                              }
                            ]
                          },
                          {
                            "prim": "GET"
                          },
                          {
                            "prim": "IF_NONE",
                            "args": [
                              [
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "pending_ops"
                                    }
                                  ]
                                },
                                {
                                  "prim": "PUSH",
                                  "args": [
                                    {
                                      "prim": "string"
                                    },
                                    {
                                      "string": "ASSET_NOT_FOUND"
                                    }
                                  ]
                                },
                                {
                                  "prim": "PAIR"
                                },
                                {
                                  "prim": "FAILWITH"
                                }
                              ],
                              []
                            ]
                          },
                          {
                            "prim": "CDR"
                          },
                          {
                            "prim": "PUSH",
                            "args": [
                              {
                                "prim": "bool"
                              },
                              {
                                "prim": "True"
                              }
                            ]
                          },
                          {
                            "prim": "SENDER"
                          },
                          {
                            "prim": "UPDATE"
                          },
                          {
                            "prim": "SWAP"
                          },
                          {
                            "prim": "PAIR"
                          },
                          {
                            "prim": "SOME"
                          },
                          {
                            "prim": "DUP",
                            "args": [
                              {
                                "int": "4"
                              }
                            ]
                          },
                          {
                            "prim": "UPDATE"
                          },
                          {
                            "prim": "DIP",
                            "args": [
                              {
                                "int": "1"
                              },
                              [
                                {
                                  "prim": "DIG",
                                  "args": [
                                    {
                                      "int": "6"
                                    }
                                  ]
                                },
                                {
                                  "prim": "DROP",
                                  "args": [
                                    {
                                      "int": "1"
                                    }
                                  ]
                                }
                              ]
                            ]
                          },
                          {
                            "prim": "DUG",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "DROP",
                            "args": [
                              {
                                "int": "2"
                              }
                            ]
                          },
                          {
                            "prim": "PAIR",
                            "args": [
                              {
                                "int": "6"
                              }
                            ]
                          },
                          {
                            "prim": "DIG",
                            "args": [
                              {
                                "int": "1"
                              }
                            ]
                          },
                          {
                            "prim": "PAIR"
                          }
                        ]
                      ]
                    }
                  ]
                ]
              }
            ],
            [
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "6"
                  }
                ]
              },
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "2"
                  }
                ]
              },
              {
                "prim": "GET"
              },
              {
                "prim": "IF_NONE",
                "args": [
                  [
                    {
                      "prim": "PUSH",
                      "args": [
                        {
                          "prim": "string"
                        },
                        {
                          "string": "INVALID OP ID"
                        }
                      ]
                    },
                    {
                      "prim": "FAILWITH"
                    }
                  ],
                  []
                ]
              },
              {
                "prim": "PUSH",
                "args": [
                  {
                    "prim": "mutez"
                  },
                  {
                    "int": "0"
                  }
                ]
              },
              {
                "prim": "AMOUNT"
              },
              {
                "prim": "COMPARE"
              },
              {
                "prim": "EQ"
              },
              {
                "prim": "NOT"
              },
              {
                "prim": "IF",
                "args": [
                  [
                    {
                      "prim": "PUSH",
                      "args": [
                        {
                          "prim": "string"
                        },
                        {
                          "string": "TO FUND CONTRACT, PLEASE USE THE DEFAULT ENTRYPOINT"
                        }
                      ]
                    },
                    {
                      "prim": "FAILWITH"
                    }
                  ],
                  []
                ]
              },
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "4"
                  }
                ]
              },
              {
                "prim": "SENDER"
              },
              {
                "prim": "MEM"
              },
              {
                "prim": "NOT"
              },
              {
                "prim": "IF",
                "args": [
                  [
                    {
                      "prim": "PUSH",
                      "args": [
                        {
                          "prim": "string"
                        },
                        {
                          "string": "ONLY FOR SIGNERS"
                        }
                      ]
                    },
                    {
                      "prim": "FAILWITH"
                    }
                  ],
                  []
                ]
              },
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "5"
                  }
                ]
              },
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "2"
                  }
                ]
              },
              {
                "prim": "CDR"
              },
              {
                "prim": "SIZE"
              },
              {
                "prim": "COMPARE"
              },
              {
                "prim": "GE"
              },
              {
                "prim": "NOT"
              },
              {
                "prim": "IF",
                "args": [
                  [
                    {
                      "prim": "PUSH",
                      "args": [
                        {
                          "prim": "string"
                        },
                        {
                          "string": "NOT YET APPROVED"
                        }
                      ]
                    },
                    {
                      "prim": "FAILWITH"
                    }
                  ],
                  []
                ]
              },
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "7"
                  }
                ]
              },
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "3"
                  }
                ]
              },
              {
                "prim": "GET"
              },
              {
                "prim": "IF_NONE",
                "args": [
                  [
                    {
                      "prim": "PUSH",
                      "args": [
                        {
                          "prim": "string"
                        },
                        {
                          "string": "pending_ops"
                        }
                      ]
                    },
                    {
                      "prim": "PUSH",
                      "args": [
                        {
                          "prim": "string"
                        },
                        {
                          "string": "ASSET_NOT_FOUND"
                        }
                      ]
                    },
                    {
                      "prim": "PAIR"
                    },
                    {
                      "prim": "FAILWITH"
                    }
                  ],
                  []
                ]
              },
              {
                "prim": "CAR"
              },
              {
                "prim": "PUSH",
                "args": [
                  {
                    "prim": "unit"
                  },
                  {
                    "prim": "Unit"
                  }
                ]
              },
              {
                "prim": "EXEC"
              },
              {
                "prim": "DIP",
                "args": [
                  {
                    "int": "1"
                  },
                  [
                    {
                      "prim": "DIG",
                      "args": [
                        {
                          "int": "8"
                        }
                      ]
                    },
                    {
                      "prim": "DROP",
                      "args": [
                        {
                          "int": "1"
                        }
                      ]
                    }
                  ]
                ]
              },
              {
                "prim": "DUG",
                "args": [
                  {
                    "int": "8"
                  }
                ]
              },
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "7"
                  }
                ]
              },
              {
                "prim": "NONE",
                "args": [
                  {
                    "prim": "pair",
                    "args": [
                      {
                        "prim": "lambda",
                        "args": [
                          {
                            "prim": "unit"
                          },
                          {
                            "prim": "list",
                            "args": [
                              {
                                "prim": "operation"
                              }
                            ]
                          }
                        ]
                      },
                      {
                        "prim": "set",
                        "args": [
                          {
                            "prim": "address"
                          }
                        ]
                      }
                    ]
                  }
                ]
              },
              {
                "prim": "DUP",
                "args": [
                  {
                    "int": "4"
                  }
                ]
              },
              {
                "prim": "UPDATE"
              },
              {
                "prim": "DIP",
                "args": [
                  {
                    "int": "1"
                  },
                  [
                    {
                      "prim": "DIG",
                      "args": [
                        {
                          "int": "6"
                        }
                      ]
                    },
                    {
                      "prim": "DROP",
                      "args": [
                        {
                          "int": "1"
                        }
                      ]
                    }
                  ]
                ]
              },
              {
                "prim": "DUG",
                "args": [
                  {
                    "int": "6"
                  }
                ]
              },
              {
                "prim": "DROP",
                "args": [
                  {
                    "int": "2"
                  }
                ]
              },
              {
                "prim": "PAIR",
                "args": [
                  {
                    "int": "6"
                  }
                ]
              },
              {
                "prim": "DIG",
                "args": [
                  {
                    "int": "1"
                  }
                ]
              },
              {
                "prim": "PAIR"
              }
            ]
          ]
        }
      ]
    ]
  }
]`)

let storage: (PublicKeyHash.t, array<PublicKeyHash.t>, int) => Storage.t = (
  _owner,
  _signers,
  _threshold,
) =>
  %raw(`{
  "prim": "Pair",
  "args": [
    {
      "string": _owner
    },
    {
      "prim": "Pair",
      "args": [
        _signers.sort().map(signer => ({"string": signer})),
        {
          "prim": "Pair",
          "args": [
            {
              "int": _threshold.toString()
            },
            {
              "prim": "Pair",
              "args": [
                {
                  "int": "0"
                },
                {
                  "prim": "Pair",
                  "args": [
                    [],
                    []
                  ]
                }
              ]
            }
          ]
        }
      ]
    }
  ]
}`)
