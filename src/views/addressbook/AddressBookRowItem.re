/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;

module AliasDeleteButton = {
  [@react.component]
  let make = (~account: Alias.t) => {
    let (aliasRequest, deleteAlias) = StoreContext.Aliases.useDelete();

    let onPressConfirmDelete = _e => {
      deleteAlias(account.name)->Promise.ignore;
    };

    <DeleteButton.IconButton
      tooltip=(
        "addressbook_delete" ++ (account.address :> string),
        I18n.Tooltip.addressbook_delete,
      )
      modalTitle=I18n.Title.delete_contact
      onPressConfirmDelete
      request=aliasRequest
    />;
  };
};

module AliasEditButton = {
  [@react.component]
  let make = (~account: Alias.t) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <IconButton
        tooltip=(
          "addressbook_edit" ++ (account.address :> string),
          I18n.Tooltip.addressbook_edit,
        )
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
      "actionMenu": style(~marginRight=24.->dp, ~flexDirection=`row, ()),
      "button": style(~marginRight=4.->dp, ()),
    })
  );

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##account == nextProps##account
  );

[@react.component]
let make =
  memo((~account: Alias.t) => {
    let addToast = LogsContext.useToast();

    <RowItem.Bordered height=46.>
      <View style=styles##inner>
        <View style=styles##cellAlias>
          <Typography.Body1> account.name->React.string </Typography.Body1>
        </View>
        <View style=styles##cellAddress>
          <Typography.Address>
            (account.address :> string)->React.string
          </Typography.Address>
        </View>
      </View>
      <View style=styles##actionButtons>
        <ClipboardButton
          copied=I18n.Log.address
          addToast
          tooltipKey=(account.address :> string)
          data=(account.address :> string)
          style=styles##button
        />
        <QrButton
          account
          tooltipKey=(account.address :> string)
          style=styles##button
        />
      </View>
      <View style=styles##actionMenu>
        <AliasEditButton account />
        <AliasDeleteButton account />
      </View>
    </RowItem.Bordered>;
  });
