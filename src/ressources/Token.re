type t = {
  address: string,
  alias: string,
  symbol: string,
};

module Decode = {
  let record = json =>
    Json.Decode.{
      address: json |> field("address", string),
      alias: json |> field("alias", string),
      symbol: json |> field("symbol", string),
    };

  let array = json => json |> Json.Decode.array(record);
};

module Encode = {
  let record = record =>
    Json.Encode.(
      object_([
        ("address", record.address |> string),
        ("alias", record.alias |> string),
        ("symbol", record.symbol |> string),
      ])
    );

  let array = arrayRecord => arrayRecord |> Json.Encode.array(record);
};

module Transfer = {
  type t = {
    source: string,
    destination: string,
    amount: int,
  };

  let decode = json =>
    Json.Decode.{
      source: json |> field("transfer_source", string),
      destination: json |> field("transfer_destination", string),
      amount: json |> field("transfer_amount", int),
    };
};

module Approve = {
  type t = {
    address: string,
    amount: int,
  };

  let decode = json =>
    Json.Decode.{
      address: json |> field("approve_address", string),
      amount: json |> field("approve_amount", int),
    };
};

module GetBalance = {
  type t = {
    address: string,
    callback: string,
  };

  let decode = json =>
    Json.Decode.{
      address: json |> field("getBalance_address", string),
      callback: json |> field("getBalance_callback", string),
    };
};

module GetAllowance = {
  type t = {
    source: string,
    destination: string,
    callback: string,
  };

  let decode = json =>
    Json.Decode.{
      source: json |> field("getAllowance_source", string),
      destination: json |> field("getAllowance_destination", string),
      callback: json |> field("getAllowance_callback", string),
    };
};

module GetTotalSupply = {
  type t = {callback: string};

  let decode = json =>
    Json.Decode.{callback: json |> field("getTotalSupply_callback", string)};
};

type action =
  | Transfer(Transfer.t)
  | Approve(Approve.t)
  | GetBalance(GetBalance.t)
  | GetAllowance(GetAllowance.t)
  | GetTotalSupply(GetTotalSupply.t);

type operation = {
  action,
  token: string,
  tx_options: Injection.transfer_options,
  common_options: Injection.common_options,
};

let makeTransfer =
    (
      ~source,
      ~destination,
      ~amount,
      ~contract,
      ~fee=?,
      ~counter=?,
      ~gasLimit=?,
      ~storageLimit=?,
      ~burnCap=?,
      ~confirmations=?,
      ~forceLowFee=?,
      (),
    ) => {
  action: Transfer(Transfer.{source, destination, amount}),
  token: contract,
  tx_options:
    Injection.makeTransferOptions(
      ~fee,
      ~gasLimit,
      ~storageLimit,
      ~parameter=None,
      ~entrypoint=None,
      (),
    ),
  common_options:
    Injection.makeCommonOptions(
      ~fee,
      ~counter,
      ~burnCap,
      ~confirmations,
      ~forceLowFee,
      (),
    ),
};

let makeGetBalance =
    (
      address,
      callback,
      contract,
      ~fee=?,
      ~counter=?,
      ~gasLimit=?,
      ~storageLimit=?,
      ~burnCap=?,
      ~confirmations=?,
      ~forceLowFee=?,
      (),
    ) => {
  action: GetBalance(GetBalance.{address, callback}),
  token: contract,
  tx_options:
    Injection.makeTransferOptions(
      ~fee,
      ~gasLimit,
      ~storageLimit,
      ~parameter=None,
      ~entrypoint=None,
      (),
    ),
  common_options:
    Injection.makeCommonOptions(
      ~fee,
      ~counter,
      ~burnCap,
      ~confirmations,
      ~forceLowFee,
      (),
    ),
};
