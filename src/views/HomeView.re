open ReactNative;

module AccountItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "container":
          style(
            ~height=84.->dp,
            ~marginVertical=6.->dp,
            ~flexDirection=`row,
            (),
          ),
        "inner": style(~justifyContent=`spaceBetween, ()),
        "border":
          style(
            ~backgroundColor=Colors.border,
            ~width=4.->dp,
            ~marginRight=11.->dp,
            ~borderTopRightRadius=4.,
            ~borderBottomRightRadius=4.,
            (),
          ),
        "title":
          style(~color=Colors.stdText, ~fontSize=14., ~fontWeight=`bold, ()),
        "balance":
          style(~color=Colors.stdText, ~fontSize=14., ~fontWeight=`bold, ()),
        "label":
          style(~color=Colors.lowText, ~fontSize=12., ~fontWeight=`_700, ()),
        "address":
          style(~color=Colors.stdText, ~fontSize=14., ~fontWeight=`_400, ()),
      })
    );

  [@react.component]
  let make = (~account: Account.t) => {
    let balanceRequest = BalanceApiRequest.useBalance(account.address);

    <View style=styles##container>
      <View style=styles##border />
      <View style=styles##inner>
        <Text style=styles##title> account.alias->React.string </Text>
        <Text style=styles##balance>
          {switch (balanceRequest) {
           | Done(Ok(balance)) => balance->React.string
           | Done(Error(error)) => error->React.string
           | NotAsked
           | Loading =>
             <ActivityIndicator
               animating=true
               size={ActivityIndicator_Size.exact(17.)}
               color=Colors.highIcon
             />
           }}
        </Text>
        <Text style=styles##label> "Address"->React.string </Text>
        <Text style=styles##address> account.address->React.string </Text>
      </View>
    </View>;
  };
};

module SendButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~position=`absolute,
            ~right=36.->dp,
            ~top=40.->dp,
            ~alignItems=`center,
            (),
          ),
        "iconContainer":
          style(
            ~width=40.->dp,
            ~height=40.->dp,
            ~justifyContent=`center,
            ~alignItems=`center,
            ~borderRadius=20.,
            ~backgroundColor=Colors.plainIconBack,
            ~marginBottom=6.->dp,
            (),
          ),
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

    let onPressCancel = _e => {
      modal.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(ModalAction.closeModal)
      ->ignore;
    };

    <>
      <TouchableOpacity style=styles##button onPress>
        <View style=styles##iconContainer>
          <Icon name=`send size=24. color=Colors.plainIconContent />
        </View>
        <Typography.ButtonPrimary12>
          "SEND"->React.string
        </Typography.ButtonPrimary12>
      </TouchableOpacity>
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <SendView onPressCancel />
      </ModalAction>
    </>;
  };
};

module AddAccountButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~alignSelf=`flexStart,
            ~marginVertical=6.->dp,
            ~paddingVertical=6.->dp,
            (),
          ),
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

    let onPressCancel = _e => {
      modal.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(ModalAction.closeModal)
      ->ignore;
    };

    <>
      <TouchableOpacity style=styles##button onPress>
        <Typography.ButtonSecondary>
          "ADD ACCOUNT"->React.string
        </Typography.ButtonSecondary>
      </TouchableOpacity>
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <AccountCreateView onPressCancel />
      </ModalAction>
    </>;
  };
};

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

[@react.component]
let make = () => {
  let accounts = StoreContext.useAccounts();

  <>
    <Page>
      <AddAccountButton />
      {accounts->Belt.Option.mapWithDefault(<LoadingView />, accounts => {
         accounts
         ->Belt.Map.String.valuesToArray
         ->Belt.Array.map(account =>
             <AccountItem key={account.address} account />
           )
         ->React.array
       })}
    </Page>
    <SendButton />
  </>;
};
