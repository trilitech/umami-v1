let getAliasFromAddress = (address, aliases) => {
  aliases
  ->Map.String.get(address)
  ->Option.map((account: Account.t) => account.alias);
};
