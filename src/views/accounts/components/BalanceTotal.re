open ReactNative;

module Base = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~marginBottom=10.->dp, ~zIndex=3, ()),
      })
    );

  [@react.component]
  let make = (~token: option(Token.t)=?, ~renderBalance=?) => {
    let balanceTotal = StoreContext.Balance.useGetTotal();
    let balanceTokenTotal =
      StoreContext.BalanceToken.useGetTotal(
        token->Belt.Option.map(token => token.address),
      );

    let theme = ThemeContext.useTheme();

    let balanceElement =
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
      </Typography.Headline>;

    <View style=styles##container>
      <Typography.Overline1>
        "TOTAL BALANCE"->React.string
      </Typography.Overline1>
      {renderBalance->Belt.Option.mapWithDefault(balanceElement, renderBalance =>
         renderBalance(balanceElement)
       )}
    </View>;
  };
};

module WithTokenSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "containerSelector":
          style(
            ~marginTop=2.->dp,
            ~height=44.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          ),
        "tokenSelector": style(~marginLeft=12.->dp, ()),
      })
    );

  [@react.component]
  let make = (~token: option(Token.t)=?) => {
    let updateToken = StoreContext.SelectedToken.useSet();

    <Base
      ?token
      renderBalance={balanceElement => {
        <View style=styles##containerSelector>
          balanceElement
          <TokenSelector
            style=styles##tokenSelector
            selectedToken={token->Belt.Option.map(token => token.address)}
            setSelectedToken=updateToken
          />
        </View>
      }}
    />;
  };
};

include Base;
