/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
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

let styles = {
  open Style
  StyleSheet.create({"label": style(~marginBottom=6.->dp, ())})
}

type element = Alias.t

module Base = {
  @react.component
  let make = (
    ~label,
    ~value: element,
    ~items: array<element>,
    ~handleChange,
    ~disabled=?,
    ~token: option<Token.t>=?,
  ) => {
    // Update balances and avoid fetching them for each element
    let _ = switch token {
    | Some(token) => StoreContext.BalanceToken.useAll(true, token.address, token.kind)->ignore
    | None => StoreContext.Balance.useAll(true)->ignore
    }

    let renderButton = AccountElements.Selector.baseRenderButton(
      ~showAmount=Balance,
      ~forceFetch=false,
      ~token,
    )
    let renderItem = AccountElements.Selector.baseRenderItem(
      ~showAmount=Balance,
      ~forceFetch=false,
      ~token,
    )
    <FormGroup>
      <FormLabel label style={styles["label"]} />
      <View>
        <Selector
          items
          ?disabled
          getItemKey={item => (item.address :> string)}
          onValueChange=handleChange
          selectedValueKey={(value.Alias.address :> string)}
          renderButton
          renderItem
          keyPopover="formGroupAccountSelector"
        />
      </View>
    </FormGroup>
  }
}

module Implicits = {
  @react.component
  let make = (
    ~label,
    ~value: Account.t,
    ~handleChange: Umami.Account.t => unit,
    ~disabled=?,
    ~token: option<Token.t>=?,
    ~filter=_ => true,
  ) => {
    let value = Alias.fromAccount(value)
    let handleChange = a => handleChange(Alias.toAccountExn(a))
    let items =
      StoreContext.Accounts.useGetAll()
      ->PublicKeyHash.Map.valuesToArray
      ->Js.Array2.filter(x => filter(x.Account.address))
      ->Js.Array2.map(Alias.fromAccount)
      ->SortArray.stableSortBy(Alias.compare)

    <Base label value items handleChange ?disabled ?token />
  }
}

@react.component
let make = (
  ~label,
  ~value: element,
  ~handleChange,
  ~disabled=?,
  ~token: option<Token.t>=?,
  ~filter=_ => true,
) => {
  let items =
    StoreContext.useGetAccountsMultisigsAliasesAsAliases()
    ->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)
    ->PublicKeyHash.Map.valuesToArray
    ->Js.Array2.filter(x => filter(x.Alias.address))
    ->SortArray.stableSortBy(Alias.compareName)
  <Base label value items handleChange ?disabled ?token />
}
