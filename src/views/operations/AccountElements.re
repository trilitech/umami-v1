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
      "itemInSelector": style(~marginHorizontal=16.->dp, ()),
      "fixed": style(~height=44.->dp, ()),
      "inner": style(~justifyContent=`spaceBetween, ()),
      "container": style(~flexDirection=`row, ()),
      "info": style(~flexDirection=`row, ~justifyContent=`spaceBetween, ()),
      "name": style(~width=150.->dp, ()),
      "accounticon": FormStyles.accountIcon,
    })
  );

let styles =
  Style.(
    StyleSheet.create({
      "shrinkContainer":
        style(~display=`flex, ~alignItems=`center, ~flexDirection=`row, ()),
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
  let make =
      (~style as styleProp=?, ~address: PublicKeyHash.t, ~n=6, ~clipboardId=?) => {
    let address = (address :> string);
    let l = address->String.length;
    let startSlice = address->Js.String2.slice(~from=0, ~to_=n - 1);
    let endSlice = address->Js.String2.slice(~from=l - n - 1, ~to_=l - 1);

    let res = Format.sprintf("%s...%s", startSlice, endSlice);

    let addToast = LogsContext.useToast();

    <View
      style=Style.(arrayOption([|styleProp, styles##shrinkContainer->Some|]))>
      <Typography.Address> res->React.string </Typography.Address>
      {clipboardId->ReactUtils.mapOpt(clipboardId =>
         <ClipboardButton
           copied=I18n.Log.beacon_sign_payload
           tooltipKey={"shrinked-address-" ++ clipboardId->Int.toString}
           addToast
           data=address
           size=40.
         />
       )}
    </View>;
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
  type account =
    | Account(Account.t)
    | Alias(Alias.t);

  let name =
    fun
    | Account(a) => a.name
    | Alias(a) => a.name;

  let address =
    fun
    | Account(a) => a.address
    | Alias(a) => a.address;

  let icon =
    fun
    | Account(a) =>
      <AliasIcon
        style=itemStyles##accounticon
        kind={Some(Account(a.kind))}
        isHD=true
      />
    | Alias(_) => React.null;

  module Item = {
    [@react.component]
    let make =
        (
          ~style as paramStyle=?,
          ~account: account,
          ~token: option(Token.t)=?,
          ~showAmount=Balance,
          ~shrinkId=?,
        ) => {
      let fixed = shrinkId == None;

      <View
        style=Style.(
          arrayOption([|
            Some(itemStyles##container),
            fixed->Option.onlyIf(() => itemStyles##fixed),
            paramStyle,
          |])
        )>
        account->icon
        <View style=itemStyles##inner>
          <View style=itemStyles##info>
            <Typography.Subtitle2 numberOfLines=1 style=itemStyles##name>
              {account->name->React.string}
            </Typography.Subtitle2>
            {switch (showAmount) {
             | Balance =>
               <AccountInfoBalance address={account->address} ?token />
             | Nothing => React.null

             | Amount(e) => e
             }}
          </View>
          {switch (shrinkId) {
           | Some(shrinkId) =>
             <ShrinkedAddress
               clipboardId=shrinkId
               address={account->address}
             />
           | None =>
             <Typography.Address>
               (account->address :> string)->React.string
             </Typography.Address>
           }}
        </View>
      </View>;
    };
  };

  let baseRenderButton =
      (~showAmount, ~token, selectedAccount: option(Account.t), _hasError) =>
    <View style=styles##selectorContent>
      {selectedAccount->Option.mapWithDefault(<LoadingView />, account =>
         <Item
           style=itemStyles##itemInSelector
           account={Account(account)}
           showAmount
           ?token
         />
       )}
    </View>;

  let baseRenderItem = (~showAmount, ~token, account: Account.t) =>
    <Item
      style=itemStyles##itemInSelector
      account={Account(account)}
      showAmount
      ?token
    />;

  let renderButton = baseRenderButton(~showAmount=Balance, ~token=None);

  let renderItem = baseRenderItem(~showAmount=Balance, ~token=None);

  module Simple = {
    [@react.component]
    let make = (~account: Account.t, ~style=?) => {
      let accounts = StoreContext.Accounts.useGetAll();

      let updateAccount = StoreContext.SelectedAccount.useSet();

      let items =
        accounts
        ->PublicKeyHash.Map.valuesToArray
        ->SortArray.stableSortBy(Account.compareName);

      <>
        <Typography.Overline2>
          I18n.account->React.string
        </Typography.Overline2>
        <View style=styles##spacer />
        <Selector
          items
          getItemKey={account => (account.address :> string)}
          ?style
          onValueChange={value => updateAccount(value.address)}
          selectedValueKey=(account.address :> string)
          renderButton
          renderItem
          keyPopover="accountSelector"
        />
      </>;
    };
  };
};
