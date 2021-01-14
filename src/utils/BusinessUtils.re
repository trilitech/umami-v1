let formatToken = amount => {
  // int fromString then toString because there is
  // unwanted char in the string like line break
  amount
  ->Belt.Int.fromString
  ->Belt.Option.mapWithDefault("0", Belt.Int.toString);
};
