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

[@react.component]
let make = () => {
  let accounts = StoreContext.Accounts.useGetAll();
  let delegatesRequestsLoaded = StoreContext.Delegate.useGetAllLoaded();

  <View style=styles##container>
    {accounts->Map.String.size == 0
       ? <LoadingView />
       : <>
           <View style=styles##header>
             <BalanceTotal />
             <DelegateButton style=styles##button />
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
           <View style=styles##list>
             <View style=styles##listContent>
               {/* tricky because all delegateRequest are separate requests done by each delegateRowItem that all need to be mounted  */
                switch (
                  delegatesRequestsLoaded->Array.some(Option.isSome),
                  delegatesRequestsLoaded->Array.size
                  === accounts->Map.String.size,
                ) {
                | (true, _) => React.null /* at least one delegate is loaded and so at least one row will be displayed */
                | (false, false) => <LoadingView /> /* some delegate requests aren't loaded yet */
                | (false, true) =>
                  /* all delegate requests are loaded but all are none */
                  <Table.Empty>
                    I18n.t#empty_delegations->React.string
                  </Table.Empty>
                }}
               {accounts
                ->Map.String.valuesToArray
                ->SortArray.stableSortBy((a, b) =>
                    Pervasives.compare(a.alias, b.alias)
                  )
                ->Array.map(account =>
                    <DelegateRowItem key={account.address} account />
                  )
                ->React.array}
             </View>
           </View>
         </>}
  </View>;
};
