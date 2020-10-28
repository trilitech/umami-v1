open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "inner": style(~width=355.->dp, ~marginLeft=14.->dp, ()),
      "actionButtons":
        style(
          ~alignSelf=`flexEnd,
          ~flexDirection=`row,
          ~marginBottom=(-3.)->dp,
          (),
        ),
    })
  );

[@react.component]
let make = (~account: Account.t) => {
  let balanceRequest = BalanceApiRequest.useBalance(account.address);

  <RowItem height=94.>
    {({hovered}: Pressable.interactionState) => {
       <>
         <View style=styles##inner>
           <AccountInfo account balanceRequest />
         </View>
         <View
           style=Style.(
             array([|
               styles##actionButtons,
               style(~display=hovered ? `flex : `none, ()),
             |])
           )>
           <IconButton icon=`copy />
           <QrButton account balanceRequest />
         </View>
       </>;
     }}
  </RowItem>;
};
