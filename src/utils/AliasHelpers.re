let getAliasFromAddress = (address, aliases) => {
  aliases
  ->Belt.Map.String.get(address)
  ->Belt.Option.map((account: Account.t) => account.alias);
};
