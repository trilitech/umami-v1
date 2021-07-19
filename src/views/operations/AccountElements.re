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

type amountDisplay =
  | Balance
  | Nothing
  | Amount(React.element);

let itemStyles =
  Style.(
    StyleSheet.create({
      "itemInSelector": style(~marginHorizontal=20.->dp, ()),
      "inner": style(~height=44.->dp, ~justifyContent=`spaceBetween, ()),
      "info": style(~flexDirection=`row, ~justifyContent=`spaceBetween, ()),
    })
  );

let styles =
  Style.(
    StyleSheet.create({
      "selectorContent":
        style(
          ~height=66.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "spacer": style(~height=6.->dp, ()),
    })
  );

module Selector = {
  module Item = {
    [@react.component]
    let make =
        (
          ~style as paramStyle=?,
          ~account: Alias.t,
          ~token: option(Token.t)=?,
          ~showAmount=Balance,
        ) => {
      <View
        style=Style.(arrayOption([|Some(itemStyles##inner), paramStyle|]))>
        <View style=itemStyles##info>
          <Typography.Subtitle2>
            account.name->React.string
          </Typography.Subtitle2>
          {switch (showAmount) {
           | Balance => <AccountInfoBalance address={account.address} ?token />
           | Nothing => React.null
           | Amount(e) => e
           }}
        </View>
        <Typography.Address>
          (account.address :> string)->React.string
        </Typography.Address>
      </View>;
    };
  };

  let baseRenderButton =
      (~showAmount, ~token, selectedAccount: option(Account.t), _hasError) =>
    <View style=styles##selectorContent>
      {selectedAccount->Option.mapWithDefault(<LoadingView />, account =>
         <Item
           style=itemStyles##itemInSelector
           account={account->Account.toAlias}
           showAmount
           ?token
         />
       )}
    </View>;

  let baseRenderItem = (~showAmount, ~token, account: Account.t) =>
    <Item
      style=itemStyles##itemInSelector
      account={account->Account.toAlias}
      showAmount
      ?token
    />;

  let renderButton = baseRenderButton(~showAmount=Balance, ~token=None);

  let renderItem = baseRenderItem(~showAmount=Balance, ~token=None);

  module Simple = {
    [@react.component]
    let make = (~style=?) => {
      let account = StoreContext.SelectedAccount.useGet();
      let accounts = StoreContext.Accounts.useGetAll();

      let updateAccount = StoreContext.SelectedAccount.useSet();

      let items =
        accounts
        ->Map.String.valuesToArray
        ->SortArray.stableSortBy(Account.compareName);

      <>
        <Typography.Overline2>
          I18n.t#account->React.string
        </Typography.Overline2>
        <View style=styles##spacer />
        <Selector
          items
          getItemKey={account => (account.address :> string)}
          ?style
          onValueChange={value => updateAccount(value.address)}
          selectedValueKey=?{
            account->Option.map((account: Account.t) =>
              (account.address :> string)
            )
          }
          renderButton
          renderItem
          keyPopover="accountSelector"
        />
      </>;
    };
  };
};
