open ReactNative;

let balanceActivityIndicator =
  <ActivityIndicator
    animating=true
    size={ActivityIndicator_Size.exact(19.)}
    color=Colors.highIcon
  />;

module Balance = {
  [@react.component]
  let make = (~address: string) => {
    let balanceRequest = StoreContext.Balance.useLoad(address);

    switch (balanceRequest) {
    | Done(Ok(balance)) =>
      I18n.t#xtz_amount(balance->BusinessUtils.formatXTZ)->React.string
    | Done(Error(_error)) => React.null
    | NotAsked
    | Loading => balanceActivityIndicator
    };
  };
};

module BalanceToken = {
  [@react.component]
  let make = (~address: string, ~token: Token.t) => {
    let balanceTokenRequest =
      StoreContext.BalanceToken.useLoad(address, Some(token.address));

    switch (balanceTokenRequest) {
    | Done(Ok(balance)) =>
      I18n.t#amount(balance->BusinessUtils.formatXTZ, token.symbol)
      ->React.string
    | Done(Error(_error)) => React.null
    | NotAsked
    | Loading => balanceActivityIndicator
    };
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "balance": style(~height=19.->dp, ~marginBottom=2.->dp, ()),
    })
  );

[@react.component]
let make = (~address: string, ~token: option(Token.t)=?) => {
  <Typography.Subtitle3 style=styles##balance>
    {switch (token) {
     | Some(token) => <BalanceToken address token />
     | None => <Balance address />
     }}
  </Typography.Subtitle3>;
};
