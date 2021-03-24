type t = {
  index: int,
  name: string,
  derivationScheme: string,
  addresses: Js.Array.t(string),
  legacyAddress: option(string),
};
