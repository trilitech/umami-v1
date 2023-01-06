open ReTaquitoTypes

type t = {
  address: PublicKeyHash.t,
  alias: string,
  balance: ReBigNumber.t,
  chain: Network.chainId,
  signers: array<PublicKeyHash.t>,
  threshold: int,
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
