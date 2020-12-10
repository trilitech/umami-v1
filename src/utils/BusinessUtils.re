let formatMilliXTZ = mutez => {
  let mutez =
    mutez
    ->Belt.Float.fromString
    ->Belt.Option.mapWithDefault(0.0, x => x /. 1000000.0)
    ->Belt.Float.toString;
  String.contains(mutez, '.') ? mutez : mutez ++ ".0";
};

let formatXTZ = mutez => {
  let mutez =
    mutez
    ->Belt.Float.fromString
    ->Belt.Option.getWithDefault(0.0)
    ->Belt.Float.toString;
  String.contains(mutez, '.') ? mutez : mutez ++ ".0";
};

let xtz = I18n.t#xtz;

let formatToken = amount => {
  // int fromString then toString because there is
  // unwanted char in the string like line break
  amount
  ->Belt.Int.fromString
  ->Belt.Option.mapWithDefault("0", Belt.Int.toString);
};
