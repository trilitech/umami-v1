open ReactNative;

module AccountDeleteButton = {
  [@react.component]
  let make = (~account: Account.t) => {
    let (accountRequest, deleteAccount) = AccountApiRequest.useDelete();

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
      "inner": style(~width=300.->dp, ~marginLeft=14.->dp, ()),
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
  let balanceRequest = BalanceApiRequest.useLoad(account.address);

  <RowItem.Bordered height=74. style={Style.style(~zIndex, ())}>
    <View style=styles##inner> <AccountInfo account balanceRequest /> </View>
    <View style=styles##actionButtons>
      <ClipboardButton data={account.address} />
      <QrButton account balanceRequest />
    </View>
    <View style=styles##actionDelegate> <DelegateButton /> </View>
    <Menu icon=Icons.More.build>
      <Menu.Item text="Edit account" icon=Icons.Edit.build />
      <AccountDeleteButton account />
    </Menu>
  </RowItem.Bordered>;
};
