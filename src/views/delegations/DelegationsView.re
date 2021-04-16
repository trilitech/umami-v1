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
  ->Map.String.valuesToArray
  ->SortArray.stableSortBy((a, b) =>
      Pervasives.compare(a.Account.alias, b.Account.alias)
    );

module DelegateItem = {
  [@react.component]
  let make = (~account: Account.t) => {
    let delegateRequest = StoreContext.Delegate.useLoad(account.address);

    delegateRequest
    ->ApiRequest.getDoneOk
    ->Option.flatMap(x => x)
    ->ReactUtils.mapOpt(_ =>
        <DelegateRowItem key={account.address} account delegateRequest />
      );
  };
};

[@react.component]
let make = () => {
  let accounts = StoreContext.Accounts.useGetAll();
  let delegateRequests = StoreContext.Delegate.useGetAllRequests();

  <View style=styles##container>
    {accounts->Map.String.size == 0
       ? <LoadingView />
       : <>
           <View style=styles##header>
             <BalanceTotal />
             <DelegateButton
               zeroTez=false
               action={Delegate.Create(None)}
               style=styles##button
             />
             <Table.Head>
               <DelegateRowItem.CellAddress>
                 <Typography.Overline3>
                   I18n.t#delegate_column_account->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellAddress>
               <DelegateRowItem.CellAmount>
                 <Typography.Overline3>
                   I18n.t#delegate_column_initial_balance->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellAmount>
               <DelegateRowItem.CellAmount>
                 <Typography.Overline3>
                   I18n.t#delegate_column_current_balance->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellAmount>
               <DelegateRowItem.CellAddress>
                 <Typography.Overline3>
                   I18n.t#delegate_column_baker->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellAddress>
               <DelegateRowItem.CellDuration>
                 <Typography.Overline3>
                   I18n.t#delegate_column_duration->React.string
                 </Typography.Overline3>
               </DelegateRowItem.CellDuration>
               <DelegateRowItem.CellReward>
                 <Typography.Overline3>
                   I18n.t#delegate_column_last_reward->React.string
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
                    I18n.t#empty_delegations->React.string
                  </Table.Empty>
                : React.null}
             {let accountsToShow = accountsToShow(accounts);
              accountsToShow
              ->Array.map(account =>
                  <DelegateItem key={account.address} account />
                )
              ->React.array}
           </DocumentContext.ScrollView>
         </>}
  </View>;
};
