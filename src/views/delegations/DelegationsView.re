open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flex=1., ()),
      "header":
        style(
          ~marginHorizontal=Theme.pagePaddingHorizontal->dp,
          ~marginTop=Theme.pagePaddingVertical->dp,
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
          ~paddingBottom=Theme.pagePaddingVertical->dp,
          ~paddingHorizontal=Theme.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  let accounts = StoreContext.useAccounts();

  <View style=styles##container>
    {accounts->Belt.Option.mapWithDefault(<LoadingView />, accounts => {
       <>
         <View style=styles##header>
           <DelegateButton style=styles##button />
           <Table.Head>
             <DelegateRowItem.CellAddress>
               <Typography.Overline3>
                 "ACCOUNT"->React.string
               </Typography.Overline3>
             </DelegateRowItem.CellAddress>
             <DelegateRowItem.CellAmount>
               <Typography.Overline3>
                 "CURRENT BALANCE"->React.string
               </Typography.Overline3>
             </DelegateRowItem.CellAmount>
             <DelegateRowItem.CellAddress>
               <Typography.Overline3>
                 "BAKER"->React.string
               </Typography.Overline3>
             </DelegateRowItem.CellAddress>
           </Table.Head>
         </View>
         <View style=styles##list>
           <View style=styles##listContent>
             {accounts
              ->Belt.Map.String.valuesToArray
              ->Belt.SortArray.stableSortBy((a, b) =>
                  Pervasives.compare(a.alias, b.alias)
                )
              ->Belt.Array.mapWithIndex((index, account) =>
                  <DelegateRowItem
                    key={account.address}
                    account
                    zIndex={accounts->Belt.Map.String.size - index}
                  />
                )
              ->React.array}
           </View>
         </View>
       </>
     })}
  </View>;
};
