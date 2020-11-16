open ReactNative;

module AccountDeleteButton = {
  [@react.component]
  let make = (~account: Account.t) => {
    let (accountRequest, deleteAccount) = AccountApiRequest.useDelete();

    let onPressConfirmDelete = _e => {
      deleteAccount(account.alias)->ignore;
    };

    <DeleteButton
      title="Delete account?"
      titleDone="Account deleted"
      onPressConfirmDelete
      request=accountRequest
    />;
  };
};

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
  let balanceRequest = BalanceApiRequest.useLoad(account.address);

  <RowItem.Bordered height=94.>
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
           <ClipboardButton data={account.address} />
           <QrButton account balanceRequest />
           <IconButton icon=Icons.Edit.build />
           <AccountDeleteButton account />
         </View>
       </>;
     }}
  </RowItem.Bordered>;
};
