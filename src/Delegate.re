type t = {
  name: string,
  address: string,
};

let decode = json =>
  Json.Decode.{
    name: json |> field("name", string),
    address: json |> field("address", string),
  };
