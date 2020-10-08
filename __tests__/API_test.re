open TestFramework;

describe("API tests", ({testAsync}) => {
  testAsync("runs valid balance test", ({expect, callback}) => {
    module Stub = {
      let call = _ => Future.value(Ok("stub"));
    };
    module UnderTest = API.Balance(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok("stub"));
        callback();
      });
    ();
  });

  testAsync("runs invalid balance test", ({expect, callback}) => {
    module Stub = {
      let call = _ => Future.value(Error("stub"));
    };
    module UnderTest = API.Balance(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Error("stub"));
        callback();
      });
    ();
  });

  testAsync("runs empty operation list test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    let expected: array(Operation.t) = [||];
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok(expected));
        callback();
      });
    ();
  });

  testAsync("runs valid operation list test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "delegation",
            "id": "9323046000",
            "level": "704778",
            "timestamp": "2020-09-07T15:13:52Z",
            "block": "BLmmAGRe4JkyT2aD2vWoFYS6NF4e49XoXvMfcCdvbUmjve8uRkh",
            "hash": "opZj2yByVxxEg4YRsZcERMqiy9VgnJkzDGQBPdFNycxxsFBZYus",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1258",
            "counter": "191297",
            "gas_limit": "10100",
            "storage_limit": "0",
            "op_id": 0,
            "delegate": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"
          },
          {
            "type": "transaction",
            "id": "9216974000",
            "level": "696545",
            "timestamp": "2020-09-04T10:00:58Z",
            "block": "BLK27pNneaexvbE4QwgEqdsSA5NM8f8XMxbYu1ufbhWD7Dhh3eG",
            "hash": "opQTxvSXMGuZ2rkox6q7ZNQhdp9a1j1ELoVdnHcvrh5ShyocEFD",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1283",
            "counter": "191296",
            "gas_limit": "10307",
            "storage_limit": "0",
            "op_id": 0,
            "amount": "1000000",
            "destination": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "parameters": {
              "prim": "Unit"
            },
            "entrypoint": "default"
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    let expected: array(Operation.t) = [|
      {
        id: "9323046000",
        level: "704778",
        timestamp: Js.Date.fromString("2020-09-07T15:13:52Z"),
        block: "BLmmAGRe4JkyT2aD2vWoFYS6NF4e49XoXvMfcCdvbUmjve8uRkh",
        hash: "opZj2yByVxxEg4YRsZcERMqiy9VgnJkzDGQBPdFNycxxsFBZYus",
        payload:
          Business({
            source: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            fee: "1258",
            op_id: 0,
            payload:
              Delegation({
                delegate: Some("tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"),
              }),
          }),
      },
      {
        id: "9216974000",
        level: "696545",
        timestamp: Js.Date.fromString("2020-09-04T10:00:58Z"),
        block: "BLK27pNneaexvbE4QwgEqdsSA5NM8f8XMxbYu1ufbhWD7Dhh3eG",
        hash: "opQTxvSXMGuZ2rkox6q7ZNQhdp9a1j1ELoVdnHcvrh5ShyocEFD",
        payload:
          Business({
            source: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            fee: "1283",
            op_id: 0,
            payload:
              Transaction({
                amount: "1000000",
                destination: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
                parameters: Some(Js.Dict.fromArray([|("prim", "Unit")|])),
              }),
          }),
      },
    |];
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok(expected));
        callback();
      });
    ();
  });

  testAsync("runs invalid operation list test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "delegation",
            "id": "9323046000",
            "level": "704778",
            "timestamp": "2020-09-07T15:13:52Z",
            "hash": "opZj2yByVxxEg4YRsZcERMqiy9VgnJkzDGQBPdFNycxxsFBZYus",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1258",
            "counter": "191297",
            "gas_limit": "10100",
            "storage_limit": "0",
            "op_id": 0,
            "delegate": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(
          Belt.Result.Error("Expected field 'block'\n\tin array at index 0"),
        );
        callback();
      });
    ();
  });

  testAsync("runs valid reveal test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "reveal",
            "id": "1226434000",
            "level": "114452",
            "timestamp": "2020-01-15T11:34:47Z",
            "block": "BMKrQdWfmYtsGVPR7kp2HohJbWRFSB7tWZdRCARVLcNnZTQhJMR",
            "hash": "oonh7WBBK92yqFi56KENtpJouPuoLSHB3srWymv8WL6orKfo5P1",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1269",
            "counter": "191233",
            "gas_limit": "10100",
            "storage_limit": "0",
            "op_id": 0,
            "public_key": "edpkuAjG6hyZ86JJ8TWBZ5j8txMX6ySsBFBcRRgmkKVBFDf3RJXfdx"
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    let expected: array(Operation.t) = [|
      {
        id: "1226434000",
        level: "114452",
        timestamp: Js.Date.fromString("2020-01-15T11:34:47Z"),
        block: "BMKrQdWfmYtsGVPR7kp2HohJbWRFSB7tWZdRCARVLcNnZTQhJMR",
        hash: "oonh7WBBK92yqFi56KENtpJouPuoLSHB3srWymv8WL6orKfo5P1",
        payload:
          Business({
            source: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            fee: "1269",
            op_id: 0,
            payload:
              Reveal({
                public_key: "edpkuAjG6hyZ86JJ8TWBZ5j8txMX6ySsBFBcRRgmkKVBFDf3RJXfdx",
              }),
          }),
      },
    |];
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok(expected));
        callback();
      });
    ();
  });

  testAsync("runs invalid reveal test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "reveal",
            "id": "1226434000",
            "level": "114452",
            "timestamp": "2020-01-15T11:34:47Z",
            "block": "BMKrQdWfmYtsGVPR7kp2HohJbWRFSB7tWZdRCARVLcNnZTQhJMR",
            "hash": "oonh7WBBK92yqFi56KENtpJouPuoLSHB3srWymv8WL6orKfo5P1",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1269",
            "counter": "191233",
            "gas_limit": "10100",
            "storage_limit": "0",
            "op_id": 0
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(
          Belt.Result.Error("Expected field 'public_key'\n\tin array at index 0"),
        );
        callback();
      });
    ();
  });

  testAsync("runs valid transaction test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "transaction",
            "id": "9216974000",
            "level": "696545",
            "timestamp": "2020-09-04T10:00:58Z",
            "block": "BLK27pNneaexvbE4QwgEqdsSA5NM8f8XMxbYu1ufbhWD7Dhh3eG",
            "hash": "opQTxvSXMGuZ2rkox6q7ZNQhdp9a1j1ELoVdnHcvrh5ShyocEFD",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1283",
            "counter": "191296",
            "gas_limit": "10307",
            "storage_limit": "0",
            "op_id": 0,
            "amount": "1000000",
            "destination": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "parameters": {
              "prim": "Unit"
            },
            "entrypoint": "default"
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    let expected: array(Operation.t) = [|
      {
        id: "9216974000",
        level: "696545",
        timestamp: Js.Date.fromString("2020-09-04T10:00:58Z"),
        block: "BLK27pNneaexvbE4QwgEqdsSA5NM8f8XMxbYu1ufbhWD7Dhh3eG",
        hash: "opQTxvSXMGuZ2rkox6q7ZNQhdp9a1j1ELoVdnHcvrh5ShyocEFD",
        payload:
          Business({
            source: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            fee: "1283",
            op_id: 0,
            payload:
              Transaction({
                amount: "1000000",
                destination: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
                parameters: Some(Js.Dict.fromArray([|("prim", "Unit")|])),
              }),
          }),
      },
    |];
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok(expected));
        callback();
      });
    ();
  });

  testAsync("runs invalid transaction test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "transaction",
            "id": "9216974000",
            "level": "696545",
            "timestamp": "2020-09-04T10:00:58Z",
            "block": "BLK27pNneaexvbE4QwgEqdsSA5NM8f8XMxbYu1ufbhWD7Dhh3eG",
            "hash": "opQTxvSXMGuZ2rkox6q7ZNQhdp9a1j1ELoVdnHcvrh5ShyocEFD",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1283",
            "counter": "191296",
            "gas_limit": "10307",
            "storage_limit": "0",
            "op_id": 0,
            "amount": "1000000",
            "parameters": {
              "prim": "Unit"
            },
            "entrypoint": "default"
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(
          Belt.Result.Error("Expected field 'destination'\n\tin array at index 0"),
        );
        callback();
      });
    ();
  });

  testAsync("runs valid origination test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "origination",
            "id": "7553106000",
            "level": "573751",
            "timestamp": "2020-07-17T08:20:00Z",
            "block": "BLvjLexwMByGckFEqn6rAwFMwdN89vZ5L4YZ6UG43w2j2JX7tDG",
            "hash": "oou9e5pWQWZ4GDoT7fiJMwdH85TytZASJMC1VV8GbaDLNsHwoXw",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "2065",
            "counter": "191266",
            "gas_limit": "15655",
            "storage_limit": "509",
            "op_id": 0,
            "contract_address": "KT1EVkzesmiNL2GLzCn73WwiiwZf4R6AVW9x"
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    let expected: array(Operation.t) = [|
      {
        id: "7553106000",
        level: "573751",
        timestamp: Js.Date.fromString("2020-07-17T08:20:00Z"),
        block: "BLvjLexwMByGckFEqn6rAwFMwdN89vZ5L4YZ6UG43w2j2JX7tDG",
        hash: "oou9e5pWQWZ4GDoT7fiJMwdH85TytZASJMC1VV8GbaDLNsHwoXw",
        payload:
          Business({
            source: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            fee: "2065",
            op_id: 0,
            payload:
              Origination({
                delegate: None,
                contract_address: "KT1EVkzesmiNL2GLzCn73WwiiwZf4R6AVW9x",
              }),
          }),
      },
    |];
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok(expected));
        callback();
      });
    ();
  });

  testAsync("runs invalid origination test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "origination",
            "id": "7553106000",
            "level": "573751",
            "timestamp": "2020-07-17T08:20:00Z",
            "block": "BLvjLexwMByGckFEqn6rAwFMwdN89vZ5L4YZ6UG43w2j2JX7tDG",
            "hash": "oou9e5pWQWZ4GDoT7fiJMwdH85TytZASJMC1VV8GbaDLNsHwoXw",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "2065",
            "counter": "191266",
            "gas_limit": "15655",
            "storage_limit": "509",
            "op_id": 0
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(
          Belt.Result.Error("Expected field 'contract_address'\n\tin array at index 0"),
        );
        callback();
      });
    ();
  });

    testAsync("runs disabled delegation test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "delegation",
            "id": "9323046000",
            "level": "704778",
            "timestamp": "2020-09-07T15:13:52Z",
            "block": "BLmmAGRe4JkyT2aD2vWoFYS6NF4e49XoXvMfcCdvbUmjve8uRkh",
            "hash": "opZj2yByVxxEg4YRsZcERMqiy9VgnJkzDGQBPdFNycxxsFBZYus",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1258",
            "counter": "191297",
            "gas_limit": "10100",
            "storage_limit": "0",
            "op_id": 0
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    let expected: array(Operation.t) = [|
      {
        id: "9323046000",
        level: "704778",
        timestamp: Js.Date.fromString("2020-09-07T15:13:52Z"),
        block: "BLmmAGRe4JkyT2aD2vWoFYS6NF4e49XoXvMfcCdvbUmjve8uRkh",
        hash: "opZj2yByVxxEg4YRsZcERMqiy9VgnJkzDGQBPdFNycxxsFBZYus",
        payload:
          Business({
            source: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            fee: "1258",
            op_id: 0,
            payload:
              Delegation({
                delegate: None,
              }),
          }),
      },
    |];
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok(expected));
        callback();
      });
    ();
  });

  testAsync("runs valid delegation test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "type": "delegation",
            "id": "9323046000",
            "level": "704778",
            "timestamp": "2020-09-07T15:13:52Z",
            "block": "BLmmAGRe4JkyT2aD2vWoFYS6NF4e49XoXvMfcCdvbUmjve8uRkh",
            "hash": "opZj2yByVxxEg4YRsZcERMqiy9VgnJkzDGQBPdFNycxxsFBZYus",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1258",
            "counter": "191297",
            "gas_limit": "10100",
            "storage_limit": "0",
            "op_id": 0,
            "delegate": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    let expected: array(Operation.t) = [|
      {
        id: "9323046000",
        level: "704778",
        timestamp: Js.Date.fromString("2020-09-07T15:13:52Z"),
        block: "BLmmAGRe4JkyT2aD2vWoFYS6NF4e49XoXvMfcCdvbUmjve8uRkh",
        hash: "opZj2yByVxxEg4YRsZcERMqiy9VgnJkzDGQBPdFNycxxsFBZYus",
        payload:
          Business({
            source: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            fee: "1258",
            op_id: 0,
            payload:
              Delegation({
                delegate: Some("tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"),
              }),
          }),
      },
    |];
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok(expected));
        callback();
      });
    ();
  });

  testAsync("runs invalid delegation test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          {
            "id": "9323046000",
            "level": "704778",
            "timestamp": "2020-09-07T15:13:52Z",
            "block": "BLmmAGRe4JkyT2aD2vWoFYS6NF4e49XoXvMfcCdvbUmjve8uRkh",
            "hash": "opZj2yByVxxEg4YRsZcERMqiy9VgnJkzDGQBPdFNycxxsFBZYus",
            "source": "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
            "fee": "1258",
            "counter": "191297",
            "gas_limit": "10100",
            "storage_limit": "0",
            "op_id": 0
          }
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    module UnderTest = API.Operations(Stub);
    UnderTest.get(Network.Test, "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3")
    ->Future.get(result => {
        expect.value(result).toEqual(
          Belt.Result.Error("Expected field 'type'\n\tin array at index 0"),
        );
        callback();
      });
    ();
  });

  testAsync("runs valid account.delegates test", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|[
          "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
          "tz1NF7b38uQ43N4nmTHvDKpr1Qo5LF9iYawk"
        ]|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    let expected = [|
      "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
      "tz1NF7b38uQ43N4nmTHvDKpr1Qo5LF9iYawk",
    |];
    module UnderTest = API.Accounts.Delegates(Stub);
    UnderTest.get(Network.Test)
    ->Future.get(result => {
        expect.value(result).toEqual(Belt.Result.Ok(expected));
        callback();
      });
    ();
  });

  testAsync("runs invalid account.delegates tests", ({expect, callback}) => {
    module Stub = {
      let fetch = _ => {
        let data = {|"tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"|};
        Future.value(Ok(data->Json.parseOrRaise));
      };
    };
    module UnderTest = API.Accounts.Delegates(Stub);
    UnderTest.get(Network.Test)
    ->Future.get(result => {
        expect.value(result).toEqual(
          Belt.Result.Error(
            "Expected array, got \"tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3\"",
          ),
        );
        callback();
      });
    ();
  });
});