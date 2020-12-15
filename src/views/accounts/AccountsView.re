open ReactNative;

module AddAccountButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~alignSelf=`flexStart,
            ~marginBottom=10.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~paddingVertical=6.->dp,
            (),
          ),
        "icon": style(~marginRight=4.->dp, ()),
      })
    );

  [@react.component]
  let make = () => {
    let theme = ThemeContext.useTheme();

    let modal = React.useRef(Js.Nullable.null);

    let (visibleModal, setVisibleModal) = React.useState(_ => false);
    let openAction = () => setVisibleModal(_ => true);
    let closeAction = () => setVisibleModal(_ => false);

    let onPress = _e => {
      openAction();
    };

    let cancel = _e => {
      modal.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(ModalAction.closeModal)
      ->ignore;
    };

    <>
      <TouchableOpacity style=styles##button onPress>
        <Icons.Add
          size=15.5
          color={theme.colors.iconMediumEmphasis}
          style=styles##icon
        />
        <Typography.ButtonSecondary>
          I18n.t#navbar_add_account->React.string
        </Typography.ButtonSecondary>
      </TouchableOpacity>
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <AccountCreateView cancel />
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
