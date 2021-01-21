open ReactNative;

module Base = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~marginBottom=10.->dp, ~zIndex=3, ()),
        "total": style(~marginBottom=4.->dp, ()),
        "balance": style(~lineHeight=22., ~height=22.->dp, ()),
      })
    );

  [@react.component]
  let make = (~token: option(Token.t)=?, ~renderBalance=?) => {
    let balanceTotal = StoreContext.Balance.useGetTotal();
    let balanceTokenTotal =
      StoreContext.BalanceToken.useGetTotal(
        token->Option.map(token => token.address),
      );

    let theme = ThemeContext.useTheme();

    let balanceElement =
      <Typography.Headline fontWeightStyle=`black style=styles##balance>
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
      </Typography.Headline>;

    <View style=styles##container>
      <Typography.Overline1 style=styles##total>
        "TOTAL BALANCE"->React.string
      </Typography.Overline1>
      {renderBalance->Option.mapWithDefault(balanceElement, renderBalance =>
         renderBalance(balanceElement)
       )}
    </View>;
  };
};

module WithTokenSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "tokenSelector": style(~minWidth=380.->dp, ~marginBottom=0.->dp, ()),
      })
    );

  let renderButton = (balanceElement, _) =>
    <View style=TokenSelector.styles##selectorContent>
      <View style=TokenSelector.TokenItem.styles##inner> balanceElement </View>
    </View>;

  [@react.component]
  let make = (~token: option(Token.t)=?) => {
    let updateToken = StoreContext.SelectedToken.useSet();

    let tokens = StoreContext.Tokens.useGetAll();

    let displaySelector = tokens->Map.String.size > 0;

    <Base
      ?token
      renderBalance=?{
        displaySelector
          ? Some(
              balanceElement => {
                <TokenSelector
                  style=styles##tokenSelector
                  selectedToken={token->Option.map(token => token.address)}
                  setSelectedToken=updateToken
                  renderButton={renderButton(balanceElement)}
                />
              },
            )
          : None
      }
    />;
  };
};

include Base;
