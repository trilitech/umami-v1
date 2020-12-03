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
      modalTitleDone=I18n.title#delete_contact
      onPressConfirmDelete
      request=aliasRequest
    />;
  };
};

let baseCellStyle = Style.(style(~flexShrink=0., ~marginRight=18.->dp, ()));
let styles =
  Style.(
    StyleSheet.create({
      "cellAlias":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=140.->dp, ()),
        |]),
      "cellAddress":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=300.->dp, ~flexGrow=1., ()),
        |]),
      "inner":
        style(~flexDirection=`row, ~width=520.->dp, ~marginLeft=22.->dp, ()),
      "actionButtons": style(~flexDirection=`row, ~flex=1., ()),
    })
  );

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##account == nextProps##account
  );

[@react.component]
let make =
  memo((~account: Account.t, ~zIndex) => {
    let addToast = LogsContext.useToast();

    <RowItem.Bordered height=46. style={Style.style(~zIndex, ())}>
      <View style=styles##inner>
        <View style=styles##cellAlias>
          <Typography.Body1> account.alias->React.string </Typography.Body1>
        </View>
        <View style=styles##cellAddress>
          <Typography.Body1> account.address->React.string </Typography.Body1>
        </View>
      </View>
      <View style=styles##actionButtons>
        <ClipboardButton
          copied=I18n.log#address
          addToast
          data={account.address}
        />
        <QrButton account />
      </View>
      <View>
        <Menu icon=Icons.More.build size=30.>
          <Menu.Item text=I18n.t#addressbook_menu_edit icon=Icons.Edit.build />
          <AliasDeleteButton account />
        </Menu>
      </View>
    </RowItem.Bordered>;
  });
