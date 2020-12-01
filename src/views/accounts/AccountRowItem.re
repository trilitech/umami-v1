open ReactNative;

module AccountDeleteButton = {
  [@react.component]
  let make = (~account: Account.t) => {
    let (accountRequest, deleteAccount) = StoreContext.useDeleteAccount();

    let onPressConfirmDelete = _e => {
      deleteAccount(account.alias)->ignore;
    };

    <DeleteButton
      buttonText="Delete account"
      modalTitle="Delete account?"
      modalTitleDone="Account deleted"
      onPressConfirmDelete
      request=accountRequest
    />;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "inner": style(~width=320.->dp, ~marginLeft=14.->dp, ()),
      "actionButtons":
        style(
          ~alignSelf=`flexEnd,
          ~flexDirection=`row,
          ~flex=1.,
          ~marginBottom=(-3.)->dp,
          (),
        ),
      "actionDelegate": style(~marginRight=12.->dp, ()),
    })
  );

[@react.component]
let make = (~account: Account.t, ~zIndex) => {
  let balanceRequest = StoreContext.useLoadBalance(account.address);
  let delegateRequest = StoreContext.useLoadDelegate(account.address);
  let addToast = LogsContext.useToast();

  <RowItem.Bordered height=74. style={Style.style(~zIndex, ())}>
    <View style=styles##inner> <AccountInfo account balanceRequest /> </View>
    <View style=styles##actionButtons>
      <ClipboardButton
        copied=I18n.log#address
        addToast
        data={account.address}
      />
      <QrButton account balanceRequest />
    </View>
    {switch (delegateRequest) {
     | Done(Ok(delegate)) =>
       <View style=styles##actionDelegate>
         <DelegateButton account disabled={delegate->Belt.Option.isSome} />
       </View>
     | _ => React.null
     }}
    <Menu icon=Icons.More.build>
      <Menu.Item text="Edit account" icon=Icons.Edit.build />
      <AccountDeleteButton account />
    </Menu>
  </RowItem.Bordered>;
};
