/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;

module Base = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~marginBottom=10.->dp, ()),
        "total": style(~marginBottom=4.->dp, ()),
        "balance": style(~lineHeight=22., ~height=22.->dp, ()),
      })
    );

  module BalanceToken = {
    [@react.component]
    let make = (~token: Token.t, ~mapWithLoading) => {
      let balanceTokenTotal =
        StoreContext.BalanceToken.useGetTotal(token.address);

      balanceTokenTotal->mapWithLoading(balance =>
        I18n.t#amount(balance->Token.Unit.toNatString, token.symbol)
        ->React.string
      );
    };
  };

  module BalanceTez = {
    [@react.component]
    let make = (~mapWithLoading) => {
      let balanceTotal = StoreContext.Balance.useGetTotal();

      balanceTotal->mapWithLoading(balance =>
        I18n.t#tez_amount(balance->Tez.toString)->React.string
      );
    };
  };

  [@react.component]
  let make = (~token: option(Token.t)=?, ~renderBalance=?) => {
    let theme = ThemeContext.useTheme();

    let mapWithLoading = (v, f) =>
      v->Option.mapWithDefault(
        <ActivityIndicator
          animating=true
          size={ActivityIndicator_Size.exact(22.)}
          color={theme.colors.iconHighEmphasis}
        />,
        f,
      );

    let balanceElement =
      <Typography.Headline fontWeightStyle=`black style=styles##balance>
        {switch (token) {
         | Some(token) => <BalanceToken mapWithLoading token />
         | None => <BalanceTez mapWithLoading />
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

  let renderButton = (balanceElement, _, _) =>
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
                  setSelectedToken={t =>
                    updateToken(t->Option.map(t => t.TokenRepr.address))
                  }
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
