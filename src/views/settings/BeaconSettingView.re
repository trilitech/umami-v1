open ReactNative;
open SettingsComponents;

module CellBase =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=154.->dp, ~flexGrow=1., ()));
    ();
  });

module CellAddress =
  Table.MakeCell({
    let style =
      Style.(style(~flexBasis=304.->dp, ~flexGrow=3., ~flexShrink=1., ()));
    ();
  });

module CellAction =
  Table.MakeCell({
    let style =
      Style.(
        style(~flexBasis=58.->dp, ~minWidth=28.->dp, ~alignItems=`flexEnd, ())
      );
    ();
  });

let styles =
  Style.(
    StyleSheet.create({
      "inner": style(~flex=1., ()),
      "spacer": style(~height=16.->dp, ()),
      "section": style(~marginBottom=8.->dp, ()),
      "loadingContainer": style(~height=44.->dp, ~minHeight=44.->dp, ()),
      "list": style(~minHeight=44.->dp, ()),
    })
  );

module PeersSection = {
  module PeerDeleteButton = {
    [@react.component]
    let make = (~peer: ReBeacon.peerInfo) => {
      let (peerRequest, deletePeer) = StoreContext.Beacon.Peers.useDelete();
      let onPressConfirmDelete = _e => {
        deletePeer(peer)->ignore;
      };
      <DeleteButton.IconButton
        tooltip=("delete_peer_" ++ peer.id, I18n.btn#delete_token)
        modalTitle=I18n.title#delete_token
        onPressConfirmDelete
        request=peerRequest
      />;
    };
  };

  module Row = {
    [@react.component]
    let make = (~peer: ReBeacon.peerInfo) => {
      <Table.Row>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            peer.name->React.string
          </Typography.Body1>
        </CellBase>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            peer.relayServer->React.string
          </Typography.Body1>
        </CellBase>
        <CellAddress />
        <CellBase />
        <CellAction> <PeerDeleteButton peer /> </CellAction>
      </Table.Row>;
    };
  };

  [@react.component]
  let make = () => {
    let peers = StoreContext.Beacon.Peers.useGetAll();

    <View>
      <Typography.Body1 fontWeightStyle=`bold style=styles##section>
        "Peers"->React.string
      </Typography.Body1>
      <Table.Head>
        <CellBase>
          <Typography.Overline3> "NAME"->React.string </Typography.Overline3>
        </CellBase>
        <CellBase>
          <Typography.Overline3>
            "RELAY SERVER"->React.string
          </Typography.Overline3>
        </CellBase>
        <CellAddress />
        <CellBase />
        <CellAction />
      </Table.Head>
      <View style=styles##list>
        {switch (peers) {
         | NotAsked
         | Loading(_) => <LoadingView />
         | Done(Ok([||]), _) =>
           <Table.Empty> I18n.t#empty_token->React.string </Table.Empty>
         | Done(Ok(peers), _) =>
           peers->Array.map(peer => <Row key={peer.id} peer />)->React.array
         | Done(Error(error), _) =>
           <Typography.Body2 colorStyle=`error>
             error->React.string
           </Typography.Body2>
         }}
      </View>
    </View>;
  };
};

module PermissionsSection = {
  module PermissionDeleteButton = {
    [@react.component]
    let make = (~permission: ReBeacon.permissionInfo) => {
      let (permissionRequest, deletePermission) =
        StoreContext.Beacon.Permissions.useDelete();
      let onPressConfirmDelete = _e => {
        deletePermission(permission.accountIdentifier)->ignore;
      };
      <DeleteButton.IconButton
        tooltip=(
          "delete_permission_" ++ permission.accountIdentifier,
          I18n.btn#delete_token,
        )
        modalTitle=I18n.title#delete_token
        onPressConfirmDelete
        request=permissionRequest
      />;
    };
  };

  module Row = {
    [@react.component]
    let make =
        (~permission: ReBeacon.permissionInfo, ~accountAlias: option(string)) => {
      <Table.Row>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            permission.appMetadata.name->React.string
          </Typography.Body1>
        </CellBase>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            {accountAlias->Option.mapWithDefault(React.null, React.string)}
          </Typography.Body1>
        </CellBase>
        <CellAddress>
          <Typography.Address numberOfLines=1>
            permission.address->React.string
          </Typography.Address>
        </CellAddress>
        <CellBase>
          <Typography.Body1 numberOfLines=1>
            permission.network.type_->React.string
          </Typography.Body1>
        </CellBase>
        <CellAction> <PermissionDeleteButton permission /> </CellAction>
      </Table.Row>;
    };
  };

  [@react.component]
  let make = () => {
    let permissions = StoreContext.Beacon.Permissions.useGetAll();
    let aliases = StoreContext.Aliases.useGetAll();
    <View>
      <Typography.Body1 fontWeightStyle=`bold style=styles##section>
        "Permissions"->React.string
      </Typography.Body1>
      <Table.Head>
        <CellBase>
          <Typography.Overline3> "DAPP"->React.string </Typography.Overline3>
        </CellBase>
        <CellBase>
          <Typography.Overline3>
            "CONNECTED ACCOUNT"->React.string
          </Typography.Overline3>
        </CellBase>
        <CellAddress>
          <Typography.Overline3>
            "ACCOUNT ADDRESS"->React.string
          </Typography.Overline3>
        </CellAddress>
        <CellBase>
          <Typography.Overline3>
            "NETWORK"->React.string
          </Typography.Overline3>
        </CellBase>
        <CellAction />
      </Table.Head>
      <View style=styles##list>
        {switch (permissions) {
         | NotAsked
         | Loading(_) => <LoadingView />
         | Done(Ok([||]), _) =>
           <Table.Empty> I18n.t#empty_token->React.string </Table.Empty>
         | Done(Ok(permissions), _) =>
           permissions
           ->Array.map(permission => {
               let accountAlias =
                 permission.address
                 ->AliasHelpers.getAliasFromAddress(aliases);
               <Row
                 key={permission.accountIdentifier}
                 permission
                 accountAlias
               />;
             })
           ->React.array
         | Done(Error(error), _) =>
           <Typography.Body2 colorStyle=`error>
             error->React.string
           </Typography.Body2>
         }}
      </View>
    </View>;
  };
};

[@react.component]
let make = () => {
  <Block title="DAPPS">
    <View style=styles##inner>
      <PeersSection />
      <View style=styles##spacer />
      <PermissionsSection />
    </View>
  </Block>;
};
