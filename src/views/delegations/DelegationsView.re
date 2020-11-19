open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flex=1., ()),
      "header":
        style(
          ~alignItems=`flexStart,
          ~marginHorizontal=Theme.pagePaddingHorizontal->dp,
          ~marginTop=Theme.pagePaddingVertical->dp,
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
         <View style=styles##header> <DelegateButton /> </View>
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
