open ReactNative;

module AddAccountButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~alignSelf=`flexStart,
            ~marginLeft=(-6.)->dp,
            ~marginBottom=10.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = () => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <View style=styles##button>
        <ButtonAction onPress text=I18n.btn#add_account icon=Icons.Add.build />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <AccountCreateView closeAction />
      </ModalAction>
    </>;
  };
};

[@react.component]
let make = () => {
  let accounts = StoreContext.Accounts.useGetAll();
  let accountsRequest = StoreContext.Accounts.useRequest();

  let token = StoreContext.SelectedToken.useGet();

  <Page>
    {accountsRequest->ApiRequest.mapOrLoad(_ => {
       <>
         <BalanceTotal.WithTokenSelector ?token />
         <AddAccountButton />
         {accounts
          ->Belt.Map.String.valuesToArray
          ->Belt.SortArray.stableSortBy((a, b) =>
              Pervasives.compare(a.alias, b.alias)
            )
          ->Belt.Array.mapWithIndex((index, account) =>
              <AccountRowItem
                key={account.address}
                account
                ?token
                zIndex={accounts->Belt.Map.String.size - index}
              />
            )
          ->React.array}
       </>
     })}
  </Page>;
};
