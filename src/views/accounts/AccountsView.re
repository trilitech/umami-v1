open ReactNative;

module EditButton = {
  let styles =
    Style.(StyleSheet.create({"button": style(~marginTop=15.->dp, ())}));

  [@react.component]
  let make = (~editMode, ~setEditMode) => {
    let onPress = _ => setEditMode(editMode => !editMode);
    <View style=styles##button>
      <ButtonAction
        onPress
        text={editMode ? I18n.btn#done_ : I18n.btn#edit}
        icon={editMode ? Icons.List.build : Icons.Edit.build}
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
    let secrets = StoreContext.Secrets.useGetAll();

    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _ => openAction();
    <>
      <View style=styles##button>
        <ButtonAction
          onPress
          text=I18n.btn#import
          icon=Icons.Import.build
          primary=true
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ImportAccountOnboardingView
          closeAction
          existingSecretsCount={secrets->Array.size}
        />
      </ModalAction>
    </>;
  };
};

module ScanImportButton = {
  module ScanView = {
    [@react.component]
    let make = (~closeAction) => {
      let (scanRequest, scan) = StoreContext.Secrets.useScanGlobal();

      let submitPassword = (~password) => {
        scan(password)->Future.tapOk(_ => closeAction())->ignore;
      };

      <ModalFormView closing={ModalFormView.Close(closeAction)}>
        <Typography.Headline style=FormStyles.header>
          I18n.title#scan->React.string
        </Typography.Headline>
        <PasswordFormView
          loading={scanRequest->ApiRequest.isLoading}
          submitPassword
        />
      </ModalFormView>;
    };
  };

  let styles =
    Style.(StyleSheet.create({"button": style(~marginLeft=(-6.)->dp, ())}));

  [@react.component]
  let make = () => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <View style=styles##button>
        <ButtonAction
          onPress
          text=I18n.btn#scan
          icon=Icons.Scan.build
          primary=true
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ScanView closeAction />
      </ModalAction>
    </>;
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
           ->Array.keepMap(secret =>
               secret.legacyAddress
               ->Option.map(legacyAddress => (secret, legacyAddress))
             )
           ->Array.map(((secret, legacyAddress)) =>
               <SecretRowTree.AccountImportedRowItem.Umami
                 key=legacyAddress
                 address=legacyAddress
                 secret
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
  Style.(StyleSheet.create({"actionBar": style(~flexDirection=`row, ())}));

[@react.component]
let make = () => {
  let resetSecrets = StoreContext.Secrets.useResetAll();
  let accountsRequest = StoreContext.Accounts.useRequest();
  let token = StoreContext.SelectedToken.useGet();

  let (editMode, setEditMode) = React.useState(_ => false);

  <Page>
    {accountsRequest->ApiRequest.mapOrEmpty(_ => {
       <>
         <Page.Header
           right=
             {<>
                <RefreshButton
                  loading={accountsRequest->ApiRequest.isLoading}
                  onRefresh=resetSecrets
                />
                <EditButton editMode setEditMode />
              </>}>
           {editMode
              ? <BalanceTotal /> : <BalanceTotal.WithTokenSelector ?token />}
           <View style=styles##actionBar>
             {editMode
                ? <View> <AccountImportButton /> <ScanImportButton /> </View>
                : React.null}
           </View>
         </Page.Header>
         {editMode ? <AccountsTreeList /> : <AccountsFlatList ?token />}
       </>
     })}
  </Page>;
};
