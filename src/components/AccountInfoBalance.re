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
  let make = (~forceFetch, ~address: PublicKeyHash.t) => {
    let balanceRequest = StoreContext.Balance.useLoad(~forceFetch, address);
    switch (balanceRequest) {
    | Done(Ok(balance), _)
    | Loading(Some(balance)) =>
      I18n.tez_amount(balance->Tez.toString)->React.string
    | Done(Error(_error), _) =>
      I18n.tez_amount(I18n.no_balance_amount)->React.string
    | NotAsked
    | Loading(None) => <BalanceActivityIndicator />
    };
  };
};

module BalanceToken = {
  [@react.component]
  let make = (~forceFetch, ~address: PublicKeyHash.t, ~token: Token.t) => {
    let balanceTokenRequest =
      StoreContext.BalanceToken.useLoad(
        ~forceFetch,
        address,
        token.address,
        token.kind,
      );

    switch (balanceTokenRequest) {
    | Done(Ok(balance), _)
    | Loading(Some(balance: Token.Unit.t)) =>
      I18n.amount(
        balance->Token.Unit.toStringDecimals(token.decimals),
        token.symbol,
      )
      ->React.string
    | Done(Error(_error), _) =>
      I18n.amount(I18n.no_balance_amount, token.symbol)->React.string

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
let make =
    (~forceFetch=true, ~address: PublicKeyHash.t, ~token: option(Token.t)=?) => {
  <Typography.Subtitle1 fontWeightStyle=`black style=styles##balance>
    {switch (token) {
     | Some(token) => <BalanceToken forceFetch address token />
     | None => <Balance forceFetch address />
     }}
  </Typography.Subtitle1>;
};
