open ReactNative
open SettingsComponents

module CellBase = Table.MakeCell({
  let style = {
    open Style
    style(~flexBasis=154.->dp, ~flexGrow=1., ())
  }
  ()
})

module CellAction = Table.MakeCell({
  let style = {
    open Style
    style(
      ~flexBasis=58.->dp,
      ~minWidth=28.->dp,
      ~flexDirection=#row,
      ~flex=1.,
      ~marginRight=auto,
      ~justifyContent=#flexEnd,
      (),
    )
  }
  ()
})

let styles = {
  open Style
  StyleSheet.create({
    "inner": style(~flex=1., ()),
    "spacer": style(~height=16.->dp, ()),
    "section": style(~marginBottom=8.->dp, ()),
    "loadingContainer": style(~height=44.->dp, ~minHeight=44.->dp, ()),
    "list": style(~minHeight=44.->dp, ()),
    "buttonContainer": style(~alignSelf=#flexStart, ~marginBottom=4.->dp, ()),
    "button": style(~alignSelf=#flexStart, ~marginLeft=-6.->dp, ~marginBottom=10.->dp, ()),
  })
}

module ConnectDAppPairingRequestButton = {
  @react.component
  let make = () => {
    let (visibleModal, openAction, closeAction) = ModalAction.useModalActionState()

    <>
      <View style={styles["buttonContainer"]}>
        <ButtonAction
          onPress={_ => openAction()}
          text="CONNECT TO DAPP WITH PAIRING REQUEST"
          icon=Icons.Copy.build
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <BeaconConnectDAppView closeAction />
      </ModalAction>
    </>
  }
}

module ConnectDAppPairingRequestWithQRButton = {
  @react.component
  let make = () => {
    let (visibleModal, openAction, closeAction) = ModalAction.useModalActionState()

    <>
      <View style={styles["buttonContainer"]}>
        <ButtonAction
          onPress={_ => openAction()} text="CONNECT TO DAPP WITH QR CODE" icon=Icons.Qr.build
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <BeaconConnectDAppView.WithQR closeAction />
      </ModalAction>
    </>
  }
}

module PeersSection = {
  module PeerDeleteButton = {
    @react.component
    let make = (~peer: Beacon.peerInfo) => {
      let (peerRequest, deletePeer) = StoreContext.Beacon.Peers.useDelete()
      let onPressConfirmDelete = _e => deletePeer(peer)
      <DeleteButton.IconButton
        tooltip=("delete_peer_" ++ peer.id, I18n.Title.delete_beacon_peer)
        modalTitle=I18n.Title.delete_beacon_peer
        onPressConfirmDelete
        request=peerRequest
      />
    }
  }

  module PeersRemoveAllButton = {
    @react.component
    let make = () => {
      let (peerRequest, deletePeers) = StoreContext.Beacon.Peers.useDeleteAll()
      let loading = peerRequest->ApiRequest.isLoading
      let onPressConfirmDelete = _e => deletePeers()
      let (openAction, _, modal) = DeleteButton.useModal(
        ~title=I18n.Title.delete_beacon_peers,
        ~loading,
        ~onPressConfirmDelete,
        (),
      )

      let onPress = _e => openAction()
      <View style={styles["button"]}>
        <ButtonAction
          onPress text=I18n.Settings.beacon_peers_remove_all icon=Icons.Delete.build size=16.
        />
        {modal()}
      </View>
    }
  }

  module Row = {
    @react.component
    let make = (~peer: Beacon.peerInfo) =>
      <Table.Row.Bordered>
        <CellBase>
          <Typography.Body1 numberOfLines=1> {peer.name->React.string} </Typography.Body1>
        </CellBase>
        <CellBase>
          <Typography.Body1 numberOfLines=1> {peer.relayServer->React.string} </Typography.Body1>
        </CellBase>
        <CellBase />
        <CellBase />
        <CellAction> <PeerDeleteButton peer /> </CellAction>
      </Table.Row.Bordered>
  }

  @react.component
  let make = () => {
    let peers = StoreContext.Beacon.Peers.useGetAll()

    <View>
      <Typography.Body1 fontWeightStyle=#bold style={styles["section"]}>
        {I18n.Settings.beacon_peers_section->React.string}
      </Typography.Body1>
      <Table.Head>
        <CellBase>
          <Typography.Overline3>
            {I18n.Settings.beacon_peers_name->React.string}
          </Typography.Overline3>
        </CellBase>
        <CellBase>
          <Typography.Overline3>
            {I18n.Settings.beacon_peers_relay->React.string}
          </Typography.Overline3>
        </CellBase>
        <CellBase />
        <CellBase />
        <CellAction>
          {switch peers {
          | Done(Ok(peers), _) if Array.length(peers) != 0 => <PeersRemoveAllButton />
          | _ => React.null
          }}
        </CellAction>
      </Table.Head>
      <View style={styles["list"]}>
        {switch peers {
        | NotAsked
        | Loading(_) =>
          <LoadingView />
        | Done(Ok([]), _) =>
          <Table.Empty> {I18n.Settings.beacon_empty_peers->React.string} </Table.Empty>
        | Done(Ok(peers), _) => peers->Array.map(peer => <Row key=peer.id peer />)->React.array
        | Done(Error(error), _) =>
          <Typography.Body2 colorStyle=#error>
            {error->Errors.toString->React.string}
          </Typography.Body2>
        }}
      </View>
    </View>
  }
}

module PermissionsSection = {
  module PermissionDeleteButton = {
    @react.component
    let make = (~permission: Beacon.permissionInfo) => {
      let (permissionRequest, deletePermission) = StoreContext.Beacon.Permissions.useDelete()
      let onPressConfirmDelete = _e => deletePermission(permission.accountIdentifier)

      <DeleteButton.IconButton
        tooltip=(
          "delete_permission_" ++ permission.accountIdentifier,
          I18n.Title.delete_beacon_permission,
        )
        modalTitle=I18n.Title.delete_beacon_permission
        onPressConfirmDelete
        request=permissionRequest
      />
    }
  }

  module PermissionsRemoveAllButton = {
    @react.component
    let make = () => {
      let (permissionsRequest, deletePermissions) = StoreContext.Beacon.Permissions.useDeleteAll()
      let loading = permissionsRequest->ApiRequest.isLoading
      let onPressConfirmDelete = _e => deletePermissions()
      let (openAction, _, modal) = DeleteButton.useModal(
        ~title=I18n.Title.delete_beacon_permissions,
        ~loading,
        ~onPressConfirmDelete,
        (),
      )

      let onPress = _e => openAction()
      <View style={styles["button"]}>
        <ButtonAction
          onPress text=I18n.Settings.beacon_permissions_remove_all icon=Icons.Delete.build size=16.
        />
        {modal()}
      </View>
    }
  }

  module Row = {
    @react.component
    let make = (~permission: Beacon.permissionInfo, ~accountAlias: option<string>) =>
      <Table.Row.Bordered>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            {permission.appMetadata.name->React.string}
          </Typography.Body1>
        </CellBase>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            {accountAlias->Option.mapWithDefault(React.null, React.string)}
          </Typography.Body1>
        </CellBase>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            {permission.scopes->Js.Array2.joinWith(`, `)->React.string}
          </Typography.Body1>
        </CellBase>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            {permission.network.type_->React.string}
          </Typography.Body1>
        </CellBase>
        <CellAction> <PermissionDeleteButton permission /> </CellAction>
      </Table.Row.Bordered>
  }

  @react.component
  let make = () => {
    let permissions = StoreContext.Beacon.Permissions.useGetAll()
    let aliases = StoreContext.Aliases.useGetAll()
    <View>
      <Typography.Body1 fontWeightStyle=#bold style={styles["section"]}>
        {I18n.Settings.beacon_permissions_section->React.string}
      </Typography.Body1>
      <Table.Head>
        <CellBase>
          <Typography.Overline3>
            {I18n.Settings.beacon_permissions_dapp->React.string}
          </Typography.Overline3>
        </CellBase>
        <CellBase>
          <Typography.Overline3>
            {I18n.Settings.beacon_permissions_account->React.string}
          </Typography.Overline3>
        </CellBase>
        <CellBase>
          <Typography.Overline3>
            {I18n.Settings.beacon_permissions_scopes->React.string}
          </Typography.Overline3>
        </CellBase>
        <CellBase>
          <Typography.Overline3>
            {I18n.Settings.beacon_permissions_network->React.string}
          </Typography.Overline3>
        </CellBase>
        <CellAction>
          {switch permissions {
          | Done(Ok([]), _) => React.null
          | Done(Ok(permissions), _) if Array.length(permissions) != 0 =>
            <PermissionsRemoveAllButton />
          | _ => React.null
          }}
        </CellAction>
      </Table.Head>
      <View style={styles["list"]}>
        {switch permissions {
        | NotAsked
        | Loading(_) =>
          <LoadingView />
        | Done(Ok([]), _) =>
          <Table.Empty> {I18n.Settings.beacon_empty_permissions->React.string} </Table.Empty>
        | Done(Ok(permissions), _) =>
          permissions
          ->Array.map(permission => {
            let accountAlias =
              permission.address
              ->PublicKeyHash.unsafeBuild
              ->AliasHelpers.getAliasFromAddress(aliases)
              ->Option.map(a => a.name)

            <Row key=permission.accountIdentifier permission accountAlias />
          })
          ->React.array
        | Done(Error(error), _) =>
          <Typography.Body2 colorStyle=#error>
            {error->Errors.toString->React.string}
          </Typography.Body2>
        }}
      </View>
    </View>
  }
}

@react.component
let make = () =>
  <Block title=I18n.Settings.beacon_title>
    <View style={styles["inner"]}>
      <ConnectDAppPairingRequestWithQRButton />
      <ConnectDAppPairingRequestButton />
      <View style={styles["spacer"]} />
      <PeersSection />
      <View style={styles["spacer"]} />
      <PermissionsSection />
    </View>
  </Block>
