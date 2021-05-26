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

let styles =
  Style.(
    StyleSheet.create({
      "inner": style(~marginLeft=14.->dp, ()),
      "tagContainer": style(~flexDirection=`row, ()),
      "tag":
        style(
          ~width=40.->dp,
          ~height=18.->dp,
          ~marginRight=8.->dp,
          ~marginLeft=2.->dp,
          ~borderRadius=9.,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderWidth=1.,
          (),
        ),
      "alias": style(~height=20.->dp, ~marginBottom=4.->dp, ()),
      "derivation": style(~height=18.->dp, ()),
      "actionContainer":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~marginLeft=auto,
          ~marginRight=24.->dp,
          (),
        ),
      "actionIconButton": style(~marginLeft=2.->dp, ()),
      "actionButton": style(~marginLeft=9.->dp, ~marginRight=7.->dp, ()),
    })
  );

module AccountNestedRowItem = {
  module AccountEditButton = {
    [@react.component]
    let make = (~account: Account.t) => {
      let (visibleModal, openAction, closeAction) =
        ModalAction.useModalActionState();

      let onPress = _e => openAction();

      <>
        <IconButton
          icon=Icons.Edit.build
          size=34.
          iconSizeRatio={1. /. 2.}
          onPress
          style=styles##actionIconButton
        />
        <ModalAction visible=visibleModal onRequestClose=closeAction>
          <AccountFormView.Update account closeAction />
        </ModalAction>
      </>;
    };
  };

  module AccountDisplayButton = {
    [@react.component]
    let make = () => {
      let onPress = _ => Js.log("Todo : Show/Hide account");
      <IconButton
        icon=Icons.Show.build
        size=34.
        iconSizeRatio={1. /. 2.}
        style=styles##actionIconButton
        onPress
      />;
    };
  };

  [@react.component]
  let make = (~address: string, ~index: int, ~isLast=false) => {
    let account = StoreContext.Accounts.useGetFromAddress(address);

    account->ReactUtils.mapOpt(account =>
      <RowItem.Bordered height=90. isNested=true isLast>
        <View style=styles##inner>
          <Typography.Subtitle1 style=styles##alias>
            account.alias->React.string
          </Typography.Subtitle1>
          <AccountInfoBalance address={account.address} />
          <Typography.Address style=styles##derivation>
            {("/" ++ index->string_of_int)->React.string}
          </Typography.Address>
        </View>
        <View style=styles##actionContainer>
          <AccountEditButton account />
        </View>
      </RowItem.Bordered>
    );
    /*<AccountDisplayButton />*/
  };
};

module SecretExportButton = {
  [@react.component]
  let make = (~secret) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <Buttons.SubmitTertiary
        onPress
        text=I18n.btn#export
        style=styles##actionButton
      />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <SecretMnemonicView closeAction secret />
      </ModalAction>
    </>;
  };
};

module AccountImportedRowItem = {
  module AccountDeleteButton = {
    [@react.component]
    let make = (~account: Account.t) => {
      let (accountRequest, deleteAccount) = StoreContext.Accounts.useDelete();

      let onPressConfirmDelete = _e => {
        deleteAccount(account.alias)->ignore;
      };

      <DeleteButton.MenuItem
        buttonText=I18n.menu#delete_account
        modalTitle=I18n.title#delete_account
        onPressConfirmDelete
        request=accountRequest
      />;
    };
  };

  module AccountDisplayButton = {
    [@react.component]
    let make = () => {
      let onPress = _e => Js.log("Todo : Show/Hide account");

      <Menu.Item text="Show" icon=Icons.Show.build onPress />;
    };
  };

  module AccountEditButton = {
    [@react.component]
    let make = (~account: Account.t) => {
      let (visibleModal, openAction, closeAction) =
        ModalAction.useModalActionState();

      let onPress = _e => openAction();

      <>
        <Menu.Item text=I18n.menu#edit icon=Icons.Edit.build onPress />
        <ModalAction visible=visibleModal onRequestClose=closeAction>
          <AccountFormView.Update account closeAction />
        </ModalAction>
      </>;
    };
  };

  module Base = {
    [@react.component]
    let make =
        (~account: Account.t, ~tag: React.element, ~actions: React.element) => {
      <RowItem.Bordered height=66.>
        <View style={Style.array([|styles##inner, styles##tagContainer|])}>
          tag
          <View>
            <Typography.Subtitle1 style=styles##alias>
              account.alias->React.string
            </Typography.Subtitle1>
            <AccountInfoBalance address={account.address} />
          </View>
        </View>
        <View style=styles##actionContainer> actions </View>
      </RowItem.Bordered>;
    };
  };

  module Umami = {
    [@react.component]
    let make = (~address: string, ~secret) => {
      let account = StoreContext.Accounts.useGetFromAddress(address);
      let theme = ThemeContext.useTheme();

      account->ReactUtils.mapOpt(account =>
        <Base
          account
          tag={
            <View
              style=Style.(
                array([|
                  styles##tag,
                  style(~borderColor=theme.colors.borderMediumEmphasis, ()),
                |])
              )>
              <Typography.Body2 fontSize=9.7 colorStyle=`mediumEmphasis>
                I18n.label#account_umami->React.string
              </Typography.Body2>
            </View>
          }
          actions=
            {<>
               <SecretExportButton secret />
               <Menu
                 icon=Icons.More.build
                 style=styles##actionIconButton
                 keyPopover={
                   "accountImportRowItemMenuUmami" ++ account.address
                 }>
                 [|
                   <AccountEditButton account />,
                   <AccountDeleteButton account />,
                 |]
               </Menu>
             </>}
        />
      );
    };
  };

  module Cli = {
    [@react.component]
    let make = (~account: Account.t) => {
      let theme = ThemeContext.useTheme();

      <Base
        account
        tag={
          <View
            style=Style.(
              array([|
                styles##tag,
                style(~backgroundColor=theme.colors.statePressed, ()),
              |])
            )>
            <Typography.Body2 fontSize=9.7 colorStyle=`mediumEmphasis>
              I18n.label#account_cli->React.string
            </Typography.Body2>
          </View>
        }
        actions=
          {<>
             <Menu
               icon=Icons.More.build
               style=styles##actionIconButton
               keyPopover={"accountImportRowItemMenuCli" ++ account.address}>
               [|
                 <AccountEditButton account />,
                 /*<AccountDisplayButton />*/
                 <AccountDeleteButton account />,
               |]
             </Menu>
           </>}
      />;
    };
  };
};

module SecretRowItem = {
  module SecretDeleteButton = {
    [@react.component]
    let make = (~secret: Secret.t) => {
      let (secretRequest, deleteSecret) = StoreContext.Secrets.useDelete();

      let onPressConfirmDelete = _e => {
        deleteSecret(secret.index)->ignore;
      };

      <DeleteButton.MenuItem
        buttonText=I18n.menu#delete_secret
        modalTitle=I18n.title#delete_secret
        onPressConfirmDelete
        request=secretRequest
      />;
    };
  };

  module SecretEditButton = {
    [@react.component]
    let make = (~secret) => {
      let (visibleModal, openAction, closeAction) =
        ModalAction.useModalActionState();

      let onPress = _e => openAction();

      <>
        <Menu.Item text=I18n.menu#edit icon=Icons.Edit.build onPress />
        <ModalAction visible=visibleModal onRequestClose=closeAction>
          <SecretUpdateFormView secret closeAction />
        </ModalAction>
      </>;
    };
  };

  module SecretAddAccountButton = {
    [@react.component]
    let make = (~secret) => {
      let (visibleModal, openAction, closeAction) =
        ModalAction.useModalActionState();

      let onPress = _e => openAction();

      <>
        <Buttons.SubmitTertiary
          onPress
          text=I18n.btn#add_account
          style=styles##actionButton
        />
        <ModalAction visible=visibleModal onRequestClose=closeAction>
          <AccountFormView.Create closeAction secret />
        </ModalAction>
      </>;
    };
  };

  [@react.component]
  let make = (~secret: Secret.t) => {
    <RowItem.Bordered height=66.>
      <View style=styles##inner>
        <Typography.Subtitle1 style=styles##alias>
          secret.name->React.string
        </Typography.Subtitle1>
        <Typography.Address style=styles##derivation>
          secret.derivationScheme->React.string
        </Typography.Address>
      </View>
      <View style=styles##actionContainer>
        <SecretAddAccountButton secret />
        <SecretExportButton secret />
        <Menu
          icon=Icons.More.build
          style=styles##actionIconButton
          keyPopover={"secretRowItem" ++ secret.index->string_of_int}>
          [|<SecretEditButton secret />, <SecretDeleteButton secret />|]
        </Menu>
      </View>
    </RowItem.Bordered>;
  };
};

[@react.component]
let make = (~secret: Secret.t) => {
  <View>
    <SecretRowItem secret />
    {secret.addresses
     ->Array.mapWithIndex((index, address) =>
         <AccountNestedRowItem
           key=address
           address
           index
           isLast={secret.addresses->Array.size - 1 === index}
         />
       )
     ->React.array}
  </View>;
};
