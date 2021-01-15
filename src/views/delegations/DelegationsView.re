open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flex=1., ()),
      "header":
        style(
          ~marginHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          ~marginTop=LayoutConst.pagePaddingVertical->dp,
          ~zIndex=2,
          (),
        ),
      "button":
        style(
          ~alignSelf=`flexStart,
          ~marginTop=0.->dp,
          ~marginBottom=30.->dp,
          ~zIndex=2,
          (),
        ),
      "list": style(~flex=1., ~zIndex=1, ()),
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
               {accounts
                ->Map.String.valuesToArray
                ->SortArray.stableSortBy((a, b) =>
                    Pervasives.compare(a.alias, b.alias)
                  )
                ->Array.mapWithIndex((index, account) =>
                    <DelegateRowItem
                      key={account.address}
                      account
                      zIndex={accounts->Map.String.size - index}
                    />
                  )
                ->React.array}
             </View>
           </View>
         </>}
  </View>;
};
