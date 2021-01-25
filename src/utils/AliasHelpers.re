let getAliasFromAddress = (address, aliases) => {
  aliases
  ->Map.String.get(address)
  ->Option.map((account: Account.t) => account.alias);
};

let getContractAliasFromAddress = (address, aliases, tokens) => {
  let r =
    aliases
    ->Map.String.get(address)
    ->Option.map((account: Account.t) => account.alias);

  switch (r) {
  | None =>
    tokens
    ->Map.String.get(address)
    ->Option.map((token: Token.t) => I18n.t#token_contract(token.alias))
  | Some(r) => Some(r)
  };
};
