open ReactNative;

module EditButton = {
  let styles =
    Style.(StyleSheet.create({"button": style(~marginLeft=auto, ())}));

  [@react.component]
  let make = (~editMode, ~setEditMode) => {
    let onPress = _ => setEditMode(editMode => !editMode);
    <View style=styles##button>
      <ButtonAction
        onPress
        text={editMode ? I18n.btn#done_ : I18n.btn#edit}
        icon=Icons.Edit.build
      />
    </View>;
  };
};

module AccountImportButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button": style(~marginLeft=(-6.)->dp, ~marginBottom=2.->dp, ()),
      })
    );

  [@react.component]
  let make = () => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _ => openAction();
    <>
      <View style=styles##button>
        <ButtonAction
          onPress
          text="IMPORT"
          icon=Icons.Import.build
          primary=true
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ImportAccountOnboardingView closeAction />
      </ModalAction>
    </>;
  };
};

module ScanImportButton = {
  let styles =
    Style.(StyleSheet.create({"button": style(~marginLeft=(-6.)->dp, ())}));

  [@react.component]
  let make = () => {
    let onPress = _ => ();
    <View style=styles##button>
      <ButtonAction onPress text="SCAN" icon=Icons.Scan.build primary=true />
    </View>;
  };
};

module AccountsFlatList = {
  [@react.component]
  let make = (~token=?) => {
    let accounts = StoreContext.Accounts.useGetAll();
    <View>
      {accounts
       ->Map.String.valuesToArray
       ->SortArray.stableSortBy((a, b) =>
           Pervasives.compare(a.alias, b.alias)
         )
       ->Array.map(account =>
           <AccountRowItem key={account.address} account ?token />
         )
       ->React.array}
    </View>;
  };
};

module AccountsTreeList = {
  [@react.component]
  let make = () => {
    let secretsRequest = StoreContext.Secrets.useLoad();
    let accounts = StoreContext.Accounts.useGetAll();

    secretsRequest->ApiRequest.mapOrLoad(secrets => {
      let addressesInSecrets =
        secrets
        ->Array.map(secret => {
            secret.legacyAddress
            ->Option.mapWithDefault(secret.addresses, legacyAddress => {
                secret.addresses->Array.concat([|legacyAddress|])
              })
          })
        ->Array.reduce([||], (acc, arr) => acc->Array.concat(arr))
        ->Set.String.fromArray;

      let accountsNotInSecrets =
        accounts->Map.String.keep((address, _account) => {
          !addressesInSecrets->Set.String.has(address)
        });

      <>
        <View>
          {secrets
           ->Array.map(secret =>
               <SecretRowTree key={secret.index->string_of_int} secret />
             )
           ->React.array}
        </View>
        <View>
          {secrets
           ->Array.keepMap(secret => secret.legacyAddress)
           ->Array.map(legacyAddress =>
               <SecretRowTree.AccountImportedRowItem.Umami
                 key=legacyAddress
                 address=legacyAddress
               />
             )
           ->React.array}
        </View>
        <View>
          {accountsNotInSecrets
           ->Map.String.valuesToArray
           ->Array.map(account =>
               <SecretRowTree.AccountImportedRowItem.Cli
                 key={account.address}
                 account
               />
             )
           ->React.array}
        </View>
      </>;
    });
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "actionBar": style(~flexDirection=`row, ~marginBottom=10.->dp, ()),
      "refreshPosition":
        style(
          ~position=`absolute,
          ~top=LayoutConst.pagePaddingVertical->dp,
          ~right=LayoutConst.pagePaddingHorizontal->dp,
          ~height=40.->dp,
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  let resetAccounts = StoreContext.Accounts.useResetAll();
  let accountsRequest = StoreContext.Accounts.useRequest();
  let token = StoreContext.SelectedToken.useGet();

  let (editMode, setEditMode) = React.useState(_ => true);

  <Page>
    {accountsRequest->ApiRequest.mapOrEmpty(_ => {
       <>
         {editMode
            ? <BalanceTotal /> : <BalanceTotal.WithTokenSelector ?token />}
         <View style=styles##actionBar>
           {editMode
              ? <View> <AccountImportButton /> <ScanImportButton /> </View>
              : React.null}
           <EditButton editMode setEditMode />
         </View>
         {editMode ? <AccountsTreeList /> : <AccountsFlatList ?token />}
       </>
     })}
    <View style=styles##refreshPosition>
      <RefreshButton
        loading={accountsRequest->ApiRequest.isLoading}
        onRefresh=resetAccounts
      />
    </View>
  </Page>;
};
