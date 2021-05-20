open TokenRepr;
module Unit = Unit;

type t = TokenRepr.t;

module Decode = {
  let record = json =>
    Json.Decode.{
      address: json |> field("address", string),
      alias: json |> field("alias", string),
      symbol: json |> field("symbol", string),
      chain:
        (json |> field("chain", optional(string)))
        ->Option.getWithDefault(Network.edo2netChain),
    };

  let array = json => json |> Json.Decode.array(record);

  let viewer = Json.Decode.string;
};

module Encode = {
  let record = record =>
    Json.Encode.(
      object_([
        ("address", record.address |> string),
        ("alias", record.alias |> string),
        ("symbol", record.symbol |> string),
        ("chain", record.chain |> string),
      ])
    );

  let array = arrayRecord => arrayRecord |> Json.Encode.array(record);
};

type options = (
  ProtocolOptions.transferOptions,
  ProtocolOptions.commonOptions,
);

module Approve = {
  type t = {
    address: string,
    amount: Unit.t,
    token: TokenRepr.address,
    options,
  };
};

module GetBalance = {
  type t = {
    address: string,
    callback: option(string),
    token: TokenRepr.address,
    options,
  };
};

module GetAllowance = {
  type t = {
    source: string,
    destination: string,
    callback: option(string),
    token: TokenRepr.address,
    options,
  };
};

module GetTotalSupply = {
  type t = {
    callback: option(string),
    token: TokenRepr.address,
    options,
  };
};

type operation =
  | Transfer(Transfer.t)
  | Approve(Approve.t)
  | GetBalance(GetBalance.t)
  | GetAllowance(GetAllowance.t)
  | GetTotalSupply(GetTotalSupply.t);

let operationEntrypoint =
  fun
  | Transfer(_) => "transfer"
  | Approve(_) => "approve"
  | GetBalance(_) => "getBalance"
  | GetAllowance(_) => "getAllowance"
  | GetTotalSupply(_) => "getTotalSupply";

let setCallback = (op, callback) => {
  let callback = Some(callback);
  switch (op) {
  | Transfer(_) as t => t
  | Approve(_) as a => a
  | GetBalance(gb) => GetBalance({...gb, callback})
  | GetAllowance(ga) => GetAllowance({...ga, callback})
  | GetTotalSupply(gts) => GetTotalSupply({...gts, callback})
  };
};

let makeGetBalance =
    (
      address,
      contract: TokenRepr.address,
      ~fee=?,
      ~gasLimit=?,
      ~storageLimit=?,
      ~burnCap=?,
      ~forceLowFee=?,
      ~callback=?,
      (),
    ) => {
  let tx_options =
    ProtocolOptions.makeTransferOptions(
      ~fee,
      ~gasLimit,
      ~storageLimit,
      ~parameter=None,
      ~entrypoint=None,
      (),
    );
  let common_options =
    ProtocolOptions.makeCommonOptions(~fee, ~burnCap, ~forceLowFee, ());
  GetBalance(
    GetBalance.{
      address,
      callback,
      token: contract,
      options: (tx_options, common_options),
    },
  );
};
