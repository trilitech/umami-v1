/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2022 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNative

type amountDisplay =
  | Balance
  | Nothing
  | Amount(React.element)

let itemStyles = {
  open Style
  StyleSheet.create({
    "itemInSelector": style(~marginHorizontal=16.->dp, ()),
    "fixed": style(~height=44.->dp, ()),
    "inner": style(~justifyContent=#spaceBetween, ()),
    "container": style(~flexDirection=#row, ()),
    "info": style(~flexDirection=#row, ~justifyContent=#spaceBetween, ()),
    "name": style(~width=150.->dp, ()),
    "accounticon": FormStyles.accountIcon,
  })
}

let styles = {
  open Style
  StyleSheet.create({
    "shrinkContainer": style(~display=#flex, ~alignItems=#center, ~flexDirection=#row, ()),
    "selectorContent": style(
      ~height=66.->dp,
      ~flexDirection=#row,
      ~alignItems=#center,
      ~flex=1.,
      (),
    ),
    "spacer": style(~height=6.->dp, ()),
  })
}

module ShrinkedAddress = {
  @react.component
  let make = (~style as styleProp=?, ~address: PublicKeyHash.t, ~n=6, ~clipboardId=?) => {
    let shrinked = address->PublicKeyHash.getShrinked(~n, ())
    let address = (address :> string)

    let addToast = LogsContext.useToast()

    <View
      style={
        open Style
        arrayOption([styleProp, styles["shrinkContainer"]->Some])
      }>
      <Typography.Address> {shrinked->React.string} </Typography.Address>
      {clipboardId->ReactUtils.mapOpt(clipboardId =>
        <ClipboardButton
          copied=I18n.Log.beacon_sign_payload
          tooltipKey={"shrinked-address-" ++ clipboardId->Int.toString}
          addToast
          data=address
          size=40.
        />
      )}
    </View>
  }
}

module Slim = {
  let styles = {
    open Style
    StyleSheet.create({
      "address": style(~marginLeft=8.->dp, ()),
      "id_address": style(~display=#flex, ~flexDirection=#row, ~alignItems=#baseline, ()),
    })
  }

  @react.component
  let make = (~style as styleProp=?, ~id=?, ~address: PublicKeyHash.t, ~token=?, ~showAmount) =>
    <View style=?styleProp>
      <View style={styles["id_address"]}>
        {id->ReactUtils.mapOpt(id =>
          <Typography.Subtitle2> {id->Int.toString->React.string} </Typography.Subtitle2>
        )}
        <ShrinkedAddress address style={styles["address"]} />
      </View>
      {switch showAmount {
      | Balance => <AccountInfoBalance address ?token />
      | Nothing => React.null
      | Amount(e) => e
      }}
    </View>
}

module Selector = {
  type item = Alias.t

  let name = (x: item) => x.name

  let address = (x: item) => x.address

  module BaseItem = {
    @react.component
    let make = (
      ~style as paramStyle=?,
      ~account: item,
      ~forceFetch,
      ~token: option<Token.t>=?,
      ~showAmount=Balance,
      ~shrinkId=?,
    ) => {
      let fixed = shrinkId == None

      <View
        style={
          open Style
          arrayOption([
            Some(itemStyles["container"]),
            fixed->Option.onlyIf(() => itemStyles["fixed"]),
            paramStyle,
          ])
        }>
        <View style={itemStyles["inner"]}>
          <View style={itemStyles["info"]}>
            <Typography.Subtitle2 numberOfLines=1 style={itemStyles["name"]}>
              {account->name->React.string}
            </Typography.Subtitle2>
            {switch showAmount {
            | Balance => <AccountInfoBalance forceFetch address={account->address} ?token />
            | Nothing => React.null

            | Amount(e) => e
            }}
          </View>
          {switch shrinkId {
          | Some(shrinkId) => <ShrinkedAddress clipboardId=shrinkId address={account->address} />
          | None => (account->address :> string)->Typography.address
          }}
        </View>
      </View>
    }
  }

  let baseRenderButton = (
    ~showAmount,
    ~forceFetch,
    ~token,
    selectedAccount: option<item>,
    _hasError: bool,
  ) =>
    <View style={styles["selectorContent"]}>
      {selectedAccount->Option.mapWithDefault(<LoadingView />, account =>
        <BaseItem forceFetch style={itemStyles["itemInSelector"]} account showAmount ?token />
      )}
    </View>

  let baseRenderItem = (~showAmount, ~forceFetch, ~token, account: item) =>
    <BaseItem forceFetch style={itemStyles["itemInSelector"]} account showAmount ?token />

  module Simple = {
    open Style
    @react.component
    let make = (~account: item, ~style=?) => {
      let items =
        StoreContext.useGetAccountsMultisigsAliasesAsAliases()
        ->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)
        ->PublicKeyHash.Map.valuesToArray
        ->SortArray.stableSortBy(Alias.compareName)

      let updateAccount = StoreContext.SelectedAccount.useSet()
      let renderButton = baseRenderButton(~forceFetch=false, ~showAmount=Balance, ~token=None)
      let renderItem = baseRenderItem(~forceFetch=false, ~showAmount=Balance, ~token=None)

      <View style={Style.style(~width=390.->dp, ~paddingBottom=32.->dp, ())}>
        {I18n.account->Typography.overline2}
        <View style={styles["spacer"]} />
        <Selector
          items
          getItemKey={account => (account.address :> string)}
          ?style
          onValueChange={value => updateAccount(value.address)}
          selectedValueKey={(account.address :> string)}
          renderButton
          renderItem
          keyPopover="accountSelector"
        />
      </View>
    }
  }
}
