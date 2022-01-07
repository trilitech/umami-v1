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

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flex=1., ()),
      "header":
        style(
          ~marginHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          ~marginTop=LayoutConst.pagePaddingVertical->dp,
          (),
        ),
      "button":
        style(
          ~alignSelf=`flexStart,
          ~marginTop=0.->dp,
          ~marginBottom=30.->dp,
          (),
        ),
      "list": style(~flex=1., ()),
      "listContent":
        style(
          ~flex=1.,
          ~paddingTop=4.->dp,
          ~paddingBottom=LayoutConst.pagePaddingVertical->dp,
          ~paddingHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

let someDone = delegateRequests =>
  delegateRequests->Map.String.some((_, r) => r->ApiRequest.isDone);

let allNone = delegateRequests =>
  delegateRequests->Map.String.every((_, r) =>
    switch (r->ApiRequest.getDoneOk) {
    | Some(None) => true
    | _ => false
    }
  );

let accountsToShow = accounts =>
  accounts
  ->PublicKeyHash.Map.valuesToArray
  ->Array.map(((a, _)) => a)
  ->SortArray.stableSortBy(Account.compareName);

module DelegateItem = {
  [@react.component]
  let make = (~account: Account.t) => {
    let delegateRequest = StoreContext.Delegate.useLoad(account.address);

    delegateRequest
    ->ApiRequest.getDoneOk
    ->Option.flatMap(x => x)
    ->ReactUtils.mapOpt(_ =>
        <DelegateRowItem
          key=(account.address :> string)
          account
          delegateRequest
        />
      );
  };
};

[@react.component]
let make = () => {
  let accounts = StoreContext.Accounts.useGetAllWithDelegates();
  let delegateRequests = StoreContext.Delegate.useGetAllRequests();

  let items =
    accounts
    ->PublicKeyHash.Map.valuesToArray
    ->Array.keepMap(((account, delegate)) =>
        delegate->Option.isNone ? Some(account) : None
      )
    ->SortArray.stableSortBy(Account.compareName);

  let firstAccount = items->Array.get(0);

  <View style=styles##container>
    {accounts->PublicKeyHash.Map.isEmpty
       ? <LoadingView />
       : <>
           <View style=styles##header>
             <Typography.Headline style=Styles.title>
               I18n.Title.delegations->React.string
             </Typography.Headline>
             <BalanceTotal />
             {firstAccount->ReactUtils.mapOpt(firstAccount =>
                <DelegateButton
                  zeroTez=false
                  action={Delegate.Create(firstAccount, false)}
                  style=styles##button
                />
              )}
             <Table.Head>
               <DelegateRowItem.CellAddress>
                 <Typography.Overline3>
                   I18n.delegate_column_account->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellAddress>
               <DelegateRowItem.CellAmount>
                 <Typography.Overline3>
                   I18n.delegate_column_initial_balance->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellAmount>
               <DelegateRowItem.CellAmount>
                 <Typography.Overline3>
                   I18n.delegate_column_current_balance->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellAmount>
               <DelegateRowItem.CellAddress>
                 <Typography.Overline3>
                   I18n.delegate_column_baker->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellAddress>
               <DelegateRowItem.CellDuration>
                 <Typography.Overline3>
                   I18n.delegate_column_duration->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellDuration>
               <DelegateRowItem.CellReward>
                 <Typography.Overline3>
                   I18n.delegate_column_last_reward->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellReward>
               <DelegateRowItem.CellAction />
             </Table.Head>
           </View>
           <DocumentContext.ScrollView
             style=styles##list contentContainerStyle=styles##listContent>
             {!someDone(delegateRequests) ? <LoadingView /> : React.null}
             {allNone(delegateRequests)
                ? <Table.Empty>
                    I18n.empty_delegations->React.string
                  </Table.Empty>
                : React.null}
             {let accountsToShow = accountsToShow(accounts);
              accountsToShow
              ->Array.map(account =>
                  <DelegateItem key=(account.address :> string) account />
                )
              ->React.array}
           </DocumentContext.ScrollView>
         </>}
  </View>;
};
