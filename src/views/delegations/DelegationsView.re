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
      "thead":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~height=30.->dp,
          ~paddingLeft=22.->dp,
          ~borderColor="rgba(255,255,255,0.38)",
          ~borderBottomWidth=1.,
          ~zIndex=1,
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
           <View style=styles##thead>
             <View style=DelegateRowItem.styles##cellAccount>
               <Typography.Overline3>
                 "ACCOUNT"->React.string
               </Typography.Overline3>
             </View>
             <View style=DelegateRowItem.styles##cellBaker>
               <Typography.Overline3>
                 "BAKER"->React.string
               </Typography.Overline3>
             </View>
           </View>
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
