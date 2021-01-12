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

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flex=1., ()),
      "scrim":
        StyleSheet.flatten([|
          StyleSheet.absoluteFillObject,
          style(
            ~flexDirection=`row,
            ~justifyContent=`spaceAround,
            ~paddingVertical=78.->dp,
            ~paddingHorizontal=58.->dp,
            (),
          ),
        |]),
    })
  );

[@react.component]
let make = () => {
  let accounts = StoreContext.Accounts.useGetAll();
  let accountsRequest = StoreContext.Accounts.useRequest();

  let token = StoreContext.SelectedToken.useGet();
  let updateToken = StoreContext.SelectedToken.useSet();

  let theme = ThemeContext.useTheme();

  <Page>
    {switch (accountsRequest) {
     | Done(_)
     | NotAsked when accounts->Belt.Map.String.size <= 0 =>
       <View
         style=Style.(
           array([|
             styles##scrim,
             style(~backgroundColor=theme.colors.scrim, ()),
           |])
         )>
         <CreateAccountBigButton />
         <ImportAccountBigButton />
       </View>
     | _ =>
       accountsRequest->ApiRequest.mapOrLoad(_ => {
         <>
           <TokenSelector
             selectedToken={token->Belt.Option.map(token => token.address)}
             setSelectedToken=updateToken
           />
           <BalanceTotal ?token />
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
       })
     }}
  </Page>;
};
