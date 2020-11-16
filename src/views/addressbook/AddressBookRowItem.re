open ReactNative;

module AliasDeleteButton = {
  [@react.component]
  let make = (~account: Account.t) => {
    let (aliasRequest, deleteAlias) = AliasApiRequest.useDelete();

    let onPressConfirmDelete = _e => {
      deleteAlias(account.alias)->ignore;
    };

    <DeleteButton
      title="Delete contact?"
      titleDone="Contact deleted"
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
      "actionButtons": style(~flexDirection=`row, ()),
    })
  );

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##account == nextProps##account
  );

[@react.component]
let make =
  memo((~account: Account.t) => {
    <RowItem.Bordered height=46.>
      <View style=styles##inner>
        <View style=styles##cellAlias>
          <Typography.Body1> account.alias->React.string </Typography.Body1>
        </View>
        <View style=styles##cellAddress>
          <Typography.Body1> account.address->React.string </Typography.Body1>
        </View>
      </View>
      <View style=styles##actionButtons>
        <ClipboardButton data={account.address} />
        <QrButton account />
        <IconButton icon=Icons.Edit.build />
        <AliasDeleteButton account />
      </View>
    </RowItem.Bordered>
  });
