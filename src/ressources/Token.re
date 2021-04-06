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

  let viewer = Json.Decode.string;
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
  type elt = {
    destination: string,
    amount: int,
    token: string,
    tx_options: Protocol.transfer_options,
  };

  type t = {
    source: string,
    transfers: list(elt),
    common_options: Protocol.common_options,
  };
};

module Approve = {
  type t = {
    address: string,
    amount: int,
    token: string,
    options: (Protocol.transfer_options, Protocol.common_options),
  };
};

module GetBalance = {
  type t = {
    address: string,
    callback: option(string),
    token: string,
    options: (Protocol.transfer_options, Protocol.common_options),
  };
};

module GetAllowance = {
  type t = {
    source: string,
    destination: string,
    callback: option(string),
    token: string,
    options: (Protocol.transfer_options, Protocol.common_options),
  };
};

module GetTotalSupply = {
  type t = {
    callback: option(string),
    token: string,
    options: (Protocol.transfer_options, Protocol.common_options),
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

let makeSingleTransferElt =
    (~destination, ~amount, ~token, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) =>
  Transfer.{
    token,
    destination,
    amount,
    tx_options:
      Protocol.makeTransferOptions(
        ~fee,
        ~gasLimit,
        ~storageLimit,
        ~parameter=None,
        ~entrypoint=None,
        (),
      ),
  };

let makeTransfers = (~source, ~transfers, ~burnCap=?, ~forceLowFee=?, ()) => {
  let common_options =
    Protocol.makeCommonOptions(~fee=None, ~burnCap, ~forceLowFee, ());
  Transfer.{source, transfers, common_options};
};

let transfer = t => t->Transfer;

let makeSingleTransfer =
    (
      ~source,
      ~destination,
      ~amount,
      ~contract,
      ~fee=?,
      ~gasLimit=?,
      ~storageLimit=?,
      ~burnCap=?,
      ~forceLowFee=?,
      (),
    ) => {
  let common_options =
    Protocol.makeCommonOptions(~fee, ~burnCap, ~forceLowFee, ());
  let elt =
    makeSingleTransferElt(
      ~destination,
      ~amount,
      ~token=contract,
      ~fee?,
      ~gasLimit?,
      ~storageLimit?,
      (),
    );
  ();
  Transfer.{source, transfers: [elt], common_options};
};

let makeGetBalance =
    (
      address,
      contract,
      ~fee=?,
      ~gasLimit=?,
      ~storageLimit=?,
      ~burnCap=?,
      ~forceLowFee=?,
      ~callback=?,
      (),
    ) => {
  let tx_options =
    Protocol.makeTransferOptions(
      ~fee,
      ~gasLimit,
      ~storageLimit,
      ~parameter=None,
      ~entrypoint=None,
      (),
    );
  let common_options =
    Protocol.makeCommonOptions(~fee, ~burnCap, ~forceLowFee, ());
  GetBalance(
    GetBalance.{
      address,
      callback,
      token: contract,
      options: (tx_options, common_options),
    },
  );
};
