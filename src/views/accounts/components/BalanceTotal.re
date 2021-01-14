open ReactNative;

let styles =
  Style.(StyleSheet.create({"container": style(~marginBottom=10.->dp, ())}));

[@react.component]
let make = (~token: option(Token.t)=?) => {
  let balanceTotal = StoreContext.Balance.useGetTotal();
  let balanceTokenTotal =
    StoreContext.BalanceToken.useGetTotal(
      token->Belt.Option.map(token => token.address),
    );

  let theme = ThemeContext.useTheme();

  <View style=styles##container>
    <Typography.Overline1>
      "TOTAL BALANCE"->React.string
    </Typography.Overline1>
    <Typography.Headline>
      {switch (token, balanceTotal, balanceTokenTotal) {
       | (Some(token), _, Some(balanceTokenTotal)) =>
         I18n.t#amount(
           balanceTokenTotal->BusinessUtils.formatToken,
           token.symbol,
         )
         ->React.string
       | (None, Some(balanceTotal), _) =>
         I18n.t#xtz_amount(balanceTotal->ProtocolXTZ.toString)->React.string
       | _ =>
         <ActivityIndicator
           animating=true
           size={ActivityIndicator_Size.exact(22.)}
           color={theme.colors.iconHighEmphasis}
         />
       }}
    </Typography.Headline>
  </View>;
};
