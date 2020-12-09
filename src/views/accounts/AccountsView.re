open ReactNative;

module TokenSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "selector":
          style(
            ~zIndex=4,
            ~alignSelf=`flexStart,
            ~minWidth=320.->dp,
            ~marginTop=0.->dp,
            ~marginBottom=30.->dp,
            (),
          ),
        "selectorContent":
          style(
            ~height=42.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~flex=1.,
            (),
          ),
        "spacer": style(~height=6.->dp, ()),
      })
    );

  module TokenItem = {
    let styles =
      Style.(
        StyleSheet.create({
          "inner":
            style(
              ~height=22.->dp,
              ~marginHorizontal=20.->dp,
              ~justifyContent=`spaceBetween,
              (),
            ),
        })
      );

    [@react.component]
    let make = (~token: Token.t) => {
      <View style=styles##inner>
        <Typography.Subtitle2>
          token.alias->React.string
        </Typography.Subtitle2>
      </View>;
    };
  };

  let renderButton = (selectedToken: option(Token.t)) =>
    <View style=styles##selectorContent>
      {selectedToken->Belt.Option.mapWithDefault(<LoadingView />, token =>
         <TokenItem token />
       )}
    </View>;

  let renderItem = (token: Token.t) => <TokenItem token />;

  let xtzToken: Token.t = {
    address: "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    alias: "Tezos",
    currency: "XTZ",
  };

  [@react.component]
  let make = (~selectedToken, ~setSelectedToken) => {
    let tokensRequest = StoreContext.Tokens.useLoad();

    let items =
      tokensRequest
      ->ApiRequest.getDoneOk
      ->Belt.Option.mapWithDefault([||], Belt.Map.String.valuesToArray);

    let onValueChange = newValue => {
      setSelectedToken(_ =>
        newValue == xtzToken.address ? None : Some(newValue)
      );
    };

    <Selector
      style=styles##selector
      items
      getItemValue={token => token.address}
      renderButton
      onValueChange
      renderItem
      selectedValue=?selectedToken
      noneItem=xtzToken
    />;
  };
};

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
          color=Theme.colorDarkMediumEmphasis
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
            ~backgroundColor="rgba(92,92,92,0.32)",
            (),
          ),
        |]),
    })
  );

[@react.component]
let make = () => {
  let accounts = StoreContext.Accounts.useGetAll();
  let accountsRequest = StoreContext.Accounts.useRequest();

  let (selectedToken, setSelectedToken) = React.useState(_ => None);

  let token = StoreContext.Tokens.useGet(selectedToken);

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
           <TokenSelector selectedToken setSelectedToken />
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
