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
