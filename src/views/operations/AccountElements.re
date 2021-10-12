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
      "name": style(~width=150.->dp, ()),
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

module ShrinkedAddress = {
  [@react.component]
  let make = (~style as styleProp=?, ~address: PublicKeyHash.t, ~n=6) => {
    let address = (address :> string);
    let l = address->String.length;
    let startSlice = address->Js.String2.slice(~from=0, ~to_=n - 1);
    let endSlice = address->Js.String2.slice(~from=l - n - 1, ~to_=l - 1);

    let res = Format.sprintf("%s...%s", startSlice, endSlice);

    <Typography.Address style=?styleProp>
      res->React.string
    </Typography.Address>;
  };
};

module Slim = {
  let styles =
    Style.(
      StyleSheet.create({
        "address": style(~marginLeft=8.->dp, ()),
        "id_address":
          style(
            ~display=`flex,
            ~flexDirection=`row,
            ~alignItems=`baseline,
            (),
          ),
      })
    );

  [@react.component]
  let make =
      (
        ~style as styleProp=?,
        ~id=?,
        ~address: PublicKeyHash.t,
        ~token=?,
        ~showAmount,
      ) => {
    <View style=?styleProp>
      <View style=styles##id_address>
        {id->ReactUtils.mapOpt(id => {
           <Typography.Subtitle2>
             {id->Int.toString->React.string}
           </Typography.Subtitle2>
         })}
        <ShrinkedAddress address style=styles##address />
      </View>
      {switch (showAmount) {
       | Balance => <AccountInfoBalance address ?token />
       | Nothing => React.null
       | Amount(e) => e
       }}
    </View>;
  };
};

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
          <Typography.Subtitle2 numberOfLines=1 style=itemStyles##name>
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
        ->PublicKeyHash.Map.valuesToArray
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
