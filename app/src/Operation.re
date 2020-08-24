type transaction = {
  id: string,
  time: Js.Date.t,
  sender: string,
  receiver: string,
  amount: float,
  fee: string,
};

module Decoder = {
  let transaction = json =>
    Json.Decode.{
      id: json |> field("op_hash", string),
      time: json |> field("timestamp", date),
      sender: json |> field("src", string),
      receiver: json |> field("dst", string),
      amount:
        json
        |> field("amount", string)
        |> Js.Float.fromString
        |> (x => x /. 1000000.),
      fee: json |> field("fee", string),
    };
};