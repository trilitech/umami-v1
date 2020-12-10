type t = {
  address: string,
  alias: string,
  currency: string,
};

module Decode = {
  let record = json =>
    Json.Decode.{
      address: json |> field("address", string),
      alias: json |> field("alias", string),
      currency: json |> field("currency", string),
    };

  let array = json => json |> Json.Decode.array(record);
};

module Encode = {
  let record = record =>
    Json.Encode.(
      object_([
        ("address", record.address |> string),
        ("alias", record.alias |> string),
        ("currency", record.currency |> string),
      ])
    );

  let array = arrayRecord => arrayRecord |> Json.Encode.array(record);
};
