let getAliasFromAddress = (address, aliases) => {
  aliases
  ->Belt.Map.String.get(address)
  ->Belt.Option.mapWithDefault(address, (account: Account.t) =>
      account.alias
    );
};
