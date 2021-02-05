open ReactNative;

module AccountDeleteButton = {
  [@react.component]
  let make = (~account: Account.t) => {
    let (accountRequest, deleteAccount) = StoreContext.Accounts.useDelete();

    let onPressConfirmDelete = _e => {
      deleteAccount(account.alias)->ignore;
    };

    <DeleteButton
      buttonText="Delete account"
      modalTitle="Delete account?"
      onPressConfirmDelete
      request=accountRequest
    />;
  };
};

module AccountEditButton = {
  [@react.component]
  let make = (~account: Account.t) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <Menu.Item text="Edit account" icon=Icons.Edit.build onPress />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <AccountFormView.Update account closeAction />
      </ModalAction>
    </>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "inner": style(~marginRight=10.->dp, ~marginLeft=14.->dp, ()),
      "actionButtons":
        style(~alignSelf=`flexEnd, ~flexDirection=`row, ~flex=1., ()),
      "actionDelegate": style(~marginRight=8.->dp, ()),
      "actionMenu": style(~marginRight=24.->dp, ()),
      "button": style(~marginRight=4.->dp, ()),
    })
  );

[@react.component]
let make = (~account: Account.t, ~token: option(Token.t)=?, ~zIndex) => {
  let delegateRequest = StoreContext.Delegate.useLoad(account.address);
  let addToast = LogsContext.useToast();

  <RowItem.Bordered height=90. style={Style.style(~zIndex, ())}>
    <View style=styles##inner> <AccountInfo account ?token /> </View>
    <View style=styles##actionButtons>
      <ClipboardButton
        copied=I18n.log#address
        addToast
        data={account.address}
        style=styles##button
      />
      <QrButton account style=styles##button />
    </View>
    {delegateRequest->ApiRequest.mapOkWithDefault(React.null, delegate => {
       <View style=styles##actionDelegate>
         <DelegateButton account disabled={delegate->Option.isSome} />
       </View>
     })}
    <View style=styles##actionMenu>
      <Menu icon=Icons.More.build> <AccountEditButton account /> </Menu>
    </View>
  </RowItem.Bordered>;
  /* <AccountDeleteButton account /> */
};
