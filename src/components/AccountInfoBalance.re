open ReactNative;

module BalanceActivityIndicator = {
  [@react.component]
  let make = () => {
    let theme = ThemeContext.useTheme();
    <ActivityIndicator
      animating=true
      size={ActivityIndicator_Size.exact(20.)}
      color={theme.colors.iconHighEmphasis}
    />;
  };
};

module Balance = {
  [@react.component]
  let make = (~address: string) => {
    let balanceRequest = StoreContext.Balance.useLoad(address);

    switch (balanceRequest) {
    | Done(Ok(balance), _)
    | Loading(Some(balance)) =>
      I18n.t#xtz_amount(balance->ProtocolXTZ.toString)->React.string
    | Done(Error(_error), _) => React.null
    | NotAsked
    | Loading(None) => <BalanceActivityIndicator />
    };
  };
};

module BalanceToken = {
  [@react.component]
  let make = (~address: string, ~token: Token.t) => {
    let balanceTokenRequest =
      StoreContext.BalanceToken.useLoad(address, Some(token.address));

    switch (balanceTokenRequest) {
    | Done(Ok(balance), _)
    | Loading(Some(balance: Token.Repr.t)) =>
      I18n.t#amount(balance->Token.Repr.toNatString, token.symbol)
      ->React.string
    | Done(Error(_error), _) => React.null
    | NotAsked
    | Loading(None) => <BalanceActivityIndicator />
    };
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "balance": style(~height=20.->dp, ~marginBottom=4.->dp, ()),
    })
  );

[@react.component]
let make = (~address: string, ~token: option(Token.t)=?) => {
  <Typography.Subtitle1 fontWeightStyle=`black style=styles##balance>
    {switch (token) {
     | Some(token) => <BalanceToken address token />
     | None => <Balance address />
     }}
  </Typography.Subtitle1>;
};
