open ReactNative;

module AliasDeleteButton = {
  [@react.component]
  let make = (~account: Account.t) => {
    let (aliasRequest, deleteAlias) = StoreContext.Aliases.useDelete();

    let onPressConfirmDelete = _e => {
      deleteAlias(account.alias)->ignore;
    };

    <DeleteButton
      buttonText=I18n.btn#delete_contact
      modalTitle=I18n.title#delete_contact
      onPressConfirmDelete
      request=aliasRequest
    />;
  };
};

module AliasEditButton = {
  [@react.component]
  let make = (~account: Account.t) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <Menu.Item
        text=I18n.menu#addressbook_edit
        icon=Icons.Edit.build
        onPress
      />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ContactFormView action={Edit(account)} closeAction />
      </ModalAction>
    </>;
  };
};

let baseCellStyle = Style.(style(~flexShrink=0., ~marginRight=18.->dp, ()));
let styles =
  Style.(
    StyleSheet.create({
      "cellAlias":
        StyleSheet.flatten([|baseCellStyle, style(~minWidth=140.->dp, ())|]),
      "cellAddress":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~minWidth=303.->dp, ~marginRight=10.->dp, ()),
        |]),
      "inner":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flexShrink=0.,
          ~marginLeft=22.->dp,
          (),
        ),
      "actionButtons": style(~flexDirection=`row, ~flex=1., ()),
      "actionMenu": style(~marginRight=24.->dp, ()),
      "button": style(~marginRight=4.->dp, ()),
    })
  );

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##account == nextProps##account
  );

[@react.component]
let make =
  memo((~account: Account.t) => {
    let addToast = LogsContext.useToast();

    <RowItem.Bordered height=46.>
      <View style=styles##inner>
        <View style=styles##cellAlias>
          <Typography.Body1> account.alias->React.string </Typography.Body1>
        </View>
        <View style=styles##cellAddress>
          <Typography.Address>
            account.address->React.string
          </Typography.Address>
        </View>
      </View>
      <View style=styles##actionButtons>
        <ClipboardButton
          copied=I18n.log#address
          addToast
          data={account.address}
          style=styles##button
        />
        <QrButton account style=styles##button />
      </View>
      <View style=styles##actionMenu>
        <Menu
          icon=Icons.More.build
          keyPopover={"aaddressBookRowItemMenu" ++ account.address}>
          <AliasEditButton account />
          <AliasDeleteButton account />
        </Menu>
      </View>
    </RowItem.Bordered>;
  });
