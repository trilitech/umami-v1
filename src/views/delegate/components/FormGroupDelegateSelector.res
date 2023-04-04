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

@react.component
let make = (~label, ~value: PublicKeyHash.t, ~handleChange, ~error, ~disabled) => {
  let accounts = StoreContext.AccountsMultisigs.useGetAllWithDelegates()

  let hasError = error->Option.isSome

  let items =
    accounts
    ->PublicKeyHash.Map.valuesToArray
    ->Array.keepMap(((account, delegate)) =>
      account.address == value || delegate->Option.isNone ? Some(account) : None
    )
    ->SortArray.stableSortBy(Alias.compare)

  let renderButton = AccountElements.Selector.baseRenderButton(
    ~forceFetch=false,
    ~showAmount=Balance,
    ~token=None,
  )
  let renderItem = AccountElements.Selector.baseRenderItem(
    ~forceFetch=false,
    ~showAmount=Balance,
    ~token=None,
  )

  <FormGroup>
    <FormLabel label hasError style={styles["label"]} />
    <View>
      <Selector
        items
        getItemKey={account => (account.address :> string)}
        onValueChange=handleChange
        selectedValueKey={(value :> string)}
        renderButton
        renderItem
        disabled
        keyPopover="formGroupDelegateSelector"
      />
    </View>
  </FormGroup>
}
