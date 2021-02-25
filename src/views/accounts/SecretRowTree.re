open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "inner": style(~marginLeft=14.->dp, ()),
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
          iconSizeRatio={3. /. 7.}
          onPress
          style=styles##actionIconButton
        />
        <ModalAction visible=visibleModal onRequestClose=closeAction>
          <AccountFormView.Update account closeAction />
        </ModalAction>
      </>;
    };
  };

  [@react.component]
  let make = (~address: string, ~index: int, ~zIndex, ~isLast=false) => {
    let account = StoreContext.Accounts.useGetFromAddress(address);

    account->ReactUtils.mapOpt(account =>
      <RowItem.Bordered
        height=90. style={Style.style(~zIndex, ())} isNested=true isLast>
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

  module AccountExportButton = {
    [@react.component]
    let make = () => {
      <>
        <Buttons.SubmitTertiary
          onPress={_ => ()}
          text="EXPORT"
          style=styles##actionButton
        />
      </>;
    };
  };

  [@react.component]
  let make = (~address: string, ~zIndex) => {
    let account = StoreContext.Accounts.useGetFromAddress(address);

    account->ReactUtils.mapOpt(account =>
      <RowItem.Bordered height=66. style={Style.style(~zIndex, ())}>
        <View style=styles##inner>
          <Typography.Subtitle1 style=styles##alias>
            account.alias->React.string
          </Typography.Subtitle1>
          <AccountInfoBalance address={account.address} />
        </View>
        <View style=styles##actionContainer>
          <AccountExportButton />
          <Menu icon=Icons.More.build style=styles##actionIconButton>
            <AccountEditButton account />
          </Menu>
        </View>
      </RowItem.Bordered>
    );
  };
};

module SecretRowItem = {
  module SecretEditButton = {
    [@react.component]
    let make = () => {
      <> <Menu.Item text="Edit" icon=Icons.Edit.build onPress={_ => ()} /> </>;
    };
  };

  module SecretAddAccountButton = {
    [@react.component]
    let make = () => {
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
          <AccountFormView.Create closeAction />
        </ModalAction>
      </>;
    };
  };

  module SecretExportButton = {
    [@react.component]
    let make = () => {
      <>
        <Buttons.SubmitTertiary
          onPress={_ => ()}
          text="EXPORT"
          style=styles##actionButton
        />
      </>;
    };
  };

  [@react.component]
  let make = (~secret: API.Secret.t, ~zIndex) => {
    <RowItem.Bordered height=66. style={Style.style(~zIndex, ())}>
      <View style=styles##inner>
        <Typography.Subtitle1 style=styles##alias>
          secret.name->React.string
        </Typography.Subtitle1>
        <Typography.Address style=styles##derivation>
          secret.derivationScheme->React.string
        </Typography.Address>
      </View>
      <View style=styles##actionContainer>
        <SecretAddAccountButton />
        <SecretExportButton />
        <Menu icon=Icons.More.build style=styles##actionIconButton>
          <SecretEditButton />
        </Menu>
      </View>
    </RowItem.Bordered>;
  };
};

[@react.component]
let make = (~secret: API.Secret.t, ~zIndex) => {
  <View style={Style.style(~zIndex, ())}>
    <SecretRowItem secret zIndex={secret.addresses->Array.size + 1} />
    {secret.addresses
     ->Array.mapWithIndex((index, address) =>
         <AccountNestedRowItem
           key=address
           address
           index
           zIndex={secret.addresses->Array.size - index}
           isLast={secret.addresses->Array.size - 1 === index}
         />
       )
     ->React.array}
  </View>;
};
