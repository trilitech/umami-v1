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

  <View style=styles##container>
    <Typography.Overline2>
      "TOTAL BALANCE"->React.string
    </Typography.Overline2>
    <Typography.Headline2>
      {switch (token, balanceTotal, balanceTokenTotal) {
       | (Some(token), _, Some(balanceTokenTotal)) =>
         I18n.t#amount(
           balanceTokenTotal->BusinessUtils.formatXTZ,
           token.symbol,
         )
         ->React.string
       | (None, Some(balanceTotal), _) =>
         I18n.t#xtz_amount(balanceTotal->BusinessUtils.formatXTZ)
         ->React.string
       | _ =>
         <ActivityIndicator
           animating=true
           size={ActivityIndicator_Size.exact(22.)}
           color=Colors.highIcon
         />
       }}
    </Typography.Headline2>
  </View>;
};
