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
  let make = (~handleAdd) => {
    let modal = React.useRef(Js.Nullable.null);

    let (visibleModal, setVisibleModal) = React.useState(_ => false);
    let openAction = () => setVisibleModal(_ => true);
    let closeAction = () => setVisibleModal(_ => false);

    let onPress = _e => {
      openAction();
    };

    let onPressCancel = _e => {
      modal.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(ModalAction.closeModal)
      ->ignore;
    };

    <>
      <TouchableOpacity style=styles##button onPress>
        <Icons.Add
          size=15.5
          color=Theme.colorDarkMediumEmphasis
          style=styles##icon
        />
        <Typography.ButtonSecondary>
          "ADD ACCOUNT"->React.string
        </Typography.ButtonSecondary>
      </TouchableOpacity>
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <AccountCreateView onPressCancel handleAdd />
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
            ~backgroundColor="rgba(92,92,92,0.32)",
            (),
          ),
        |]),
    })
  );

[@react.component]
let make = () => {
  let accounts = StoreContext.useAccounts();
  let refreshAccounts = StoreContext.useRefreshAccounts();
  let accountsRequest = StoreContext.useAccountsRequest();
  let handleAdd = () => refreshAccounts(~loading=false, ())->ignore;
  let handleDelete = handleAdd;

  <Page>
    {switch (accountsRequest) {
     | Done(_)
     | NotAsked when accounts->Belt.Map.String.size <= 0 =>
       <View style=styles##scrim>
         <CreateAccountBigButton />
         <ImportAccountBigButton />
       </View>
     | _ =>
       accountsRequest->ApiRequest.mapOrLoad(_ => {
         <>
           <AddAccountButton handleAdd />
           {accounts
            ->Belt.Map.String.valuesToArray
            ->Belt.SortArray.stableSortBy((a, b) =>
                Pervasives.compare(a.alias, b.alias)
              )
            ->Belt.Array.mapWithIndex((index, account) =>
                <AccountRowItem
                  key={account.address}
                  account
                  handleDelete
                  zIndex={accounts->Belt.Map.String.size - index}
                />
              )
            ->React.array}
         </>
       })
     }}
  </Page>;
};
