let formatToken = amount => {
  // int fromString then toString because there is
  // unwanted char in the string like line break
  amount
  ->Int.fromString
  ->Option.mapWithDefault("0", Int.toString);
};
