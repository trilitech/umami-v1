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

  testAsync("runs valid operation list tests", ({expect, callback}) => {
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

  testAsync("runs invalid operation list tests", ({expect, callback}) => {
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

  testAsync("runs valid account.delegates tests", ({expect, callback}) => {
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
        expect.value(result).toEqual(Belt.Result.Error("Expected array, got \"tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3\""));
        callback();
      });
    ();
  });
});
