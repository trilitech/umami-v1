type t = {
  name: string,
  address: string,
};

let decode = json =>
  Json.Decode.{
    name: json |> field("name", string),
    address: json |> field("address", string),
  };

type action =
  | Create(option(Account.t))
  | Edit(Account.t, string)
  | Delete(Account.t, string);

let account = action =>
  switch (action) {
  | Create(a) => a
  | Edit(a, _) => Some(a)
  | Delete(a, _) => Some(a)
  };
