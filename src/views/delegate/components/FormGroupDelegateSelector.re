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
open UmamiCommon;

let styles =
  Style.(StyleSheet.create({"label": style(~marginBottom=6.->dp, ())}));

[@react.component]
let make = (~label, ~value: string, ~handleChange, ~error, ~disabled) => {
  let accounts = StoreContext.Accounts.useGetAllWithDelegates();

  let hasError = error->Option.isSome;

  let items =
    accounts
    ->Map.String.valuesToArray
    ->Array.keepMap(((account, delegate)) =>
        delegate->Option.isNone || disabled ? Some(account) : None
      )
    ->SortArray.stableSortBy((a, b) => Pervasives.compare(a.alias, b.alias));

  React.useEffect2(
    () => {
      if (value == "") {
        let firstItem = items->Array.get(0);
        firstItem->Lib.Option.iter(account => account->handleChange);
      };
      None;
    },
    (value, items),
  );

  <FormGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <Selector
        items
        getItemKey={account => account.address}
        onValueChange=handleChange
        selectedValueKey=value
        renderButton=AccountSelector.renderButton
        renderItem=AccountSelector.renderItem
        disabled
        keyPopover="formGroupDelegateSelector"
      />
    </View>
  </FormGroup>;
};
