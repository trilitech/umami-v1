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
      "rowItem": style(~paddingLeft=14.->dp, ()),
      "tagContainer": style(~flexDirection=`row, ()),
      "tag": style(~marginRight=14.->dp, ()),
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
  let make = (~address: PublicKeyHash.t, ~index: int, ~isLast=false) => {
    let account = StoreContext.Accounts.useGetFromAddress(address);

    account->ReactUtils.mapOpt(account =>
      <RowItem.Bordered
        innerStyle=styles##rowItem height=90. isNested=true isLast>
        <View>
          <Typography.Subtitle1 style=styles##alias>
            account.name->React.string
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
        text=I18n.Btn.export
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
        deleteAccount(account.name)->Promise.ignore;
      };

      <DeleteButton.MenuItem
        buttonText=I18n.Menu.delete_account
        modalTitle=I18n.Title.delete_account
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
        <Menu.Item text=I18n.Menu.edit icon=Icons.Edit.build onPress />
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
      <RowItem.Bordered innerStyle=styles##rowItem height=66.>
        <View style=styles##tagContainer>
          tag
          <View>
            <Typography.Subtitle1 style=styles##alias>
              account.name->React.string
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
    let make = (~address: PublicKeyHash.t, ~secret) => {
      let account = StoreContext.Accounts.useGetFromAddress(address);
      account->ReactUtils.mapOpt(account =>
        <Base
          account
          tag={<Tag style=styles##tag content=I18n.Label.account_umami />}
          actions=
            {<>
               <SecretExportButton secret />
               <Menu
                 icon=Icons.More.build
                 style=styles##actionIconButton
                 keyPopover={
                   "accountImportRowItemMenuUmami"
                   ++ (account.address :> string)
                 }>
                 [|
                   <AccountEditButton key="accountEditButton" account />,
                   <AccountDeleteButton key="accountDeleteButton" account />,
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
      <Base
        account
        tag={<Tag style=styles##tag content=I18n.Label.account_cli />}
        actions=
          {<>
             <Menu
               icon=Icons.More.build
               style=styles##actionIconButton
               keyPopover={
                 "accountImportRowItemMenuCli" ++ (account.address :> string)
               }>
               [|
                 <AccountEditButton key="accountEditButton" account />,
                 <AccountDeleteButton key="accountDeleteButton" account />,
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
    let make = (~secret: Secret.derived) => {
      let (secretRequest, deleteSecret) = StoreContext.Secrets.useDelete();

      let onPressConfirmDelete = _e => {
        deleteSecret(secret.index)->Promise.ignore;
      };

      <DeleteButton.MenuItem
        buttonText=I18n.Menu.delete_secret
        modalTitle=I18n.Title.delete_secret
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
        <Menu.Item text=I18n.Menu.edit icon=Icons.Edit.build onPress />
        <ModalAction visible=visibleModal onRequestClose=closeAction>
          <SecretUpdateFormView secret closeAction />
        </ModalAction>
      </>;
    };
  };

  module SecretScanButton = {
    [@react.component]
    let make = (~secret) => {
      let (visibleModal, openAction, closeAction) =
        ModalAction.useModalActionState();

      let onPress = _e => openAction();

      let isLedger = secret.Secret.secret.kind == Ledger;
      <>
        <Buttons.SubmitTertiary
          onPress
          text=I18n.Btn.scan
          style=styles##actionButton
        />
        <ModalAction visible=visibleModal onRequestClose=closeAction>
          {isLedger
             ? <LedgerScan
                 secret={secret.Secret.secret}
                 closeAction
                 index={secret.Secret.index}
               />
             // MnemonicScan is not used actually here, but the action exists
             // nonetheless
             : <MnemonicScan
                 secret={secret.Secret.secret}
                 closeAction
                 index={secret.Secret.index}
               />}
        </ModalAction>
      </>;
    };
  };

  module SecretScanMenuItem = {
    [@react.component]
    let make = (~secret) => {
      let (visibleModal, openAction, closeAction) =
        ModalAction.useModalActionState();

      let onPress = _e => openAction();

      let isLedger = secret.Secret.secret.kind == Ledger;
      !isLedger
        ? <>
            <Menu.Item text=I18n.Menu.scan icon=Icons.Scan.build onPress />
            <ModalAction visible=visibleModal onRequestClose=closeAction>
              <MnemonicScan
                secret={secret.Secret.secret}
                closeAction
                index={secret.Secret.index}
              />
            </ModalAction>
          </>
        : <> </>;
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
          text=I18n.Btn.add_account
          style=styles##actionButton
        />
        <ModalAction visible=visibleModal onRequestClose=closeAction>
          <AccountFormView.Create closeAction secret />
        </ModalAction>
      </>;
    };
  };

  [@react.component]
  let make = (~secret: Secret.derived) => {
    <RowItem.Bordered innerStyle=styles##rowItem height=66.>
      {<Tag style=styles##tag content=I18n.hw />
       ->ReactUtils.onlyWhen(secret.secret.kind == Secret.Ledger)}
      <View>
        <Typography.Subtitle1 style=styles##alias>
          secret.secret.name->React.string
        </Typography.Subtitle1>
        <Typography.Address style=styles##derivation>
          {secret.secret.derivationPath
           ->DerivationPath.Pattern.toString
           ->React.string}
        </Typography.Address>
      </View>
      <View style=styles##actionContainer>
        {<SecretScanButton secret />
         ->ReactUtils.onlyWhen(secret.secret.kind == Secret.Ledger)}
        <SecretAddAccountButton secret />
        {<SecretExportButton secret />
         ->ReactUtils.onlyWhen(secret.secret.kind != Secret.Ledger)}
        <Menu
          icon=Icons.More.build
          style=styles##actionIconButton
          keyPopover={"secretRowItem" ++ secret.index->string_of_int}>
          [|
            <SecretEditButton key="secretEditButton" secret />,
            <SecretScanMenuItem key="secretScanMenuItem" secret />,
            <SecretDeleteButton key="secretDeleteButton" secret />,
          |]
        </Menu>
      </View>
    </RowItem.Bordered>;
  };
};

[@react.component]
let make = (~secret: Secret.derived) => {
  <View>
    <SecretRowItem secret />
    {secret.secret.addresses
     ->Array.mapWithIndex((index, address) =>
         <AccountNestedRowItem
           key=(address :> string)
           address
           index
           isLast={secret.secret.addresses->Array.size - 1 === index}
         />
       )
     ->React.array}
  </View>;
};
