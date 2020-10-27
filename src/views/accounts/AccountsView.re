open ReactNative;

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
        <Typography.ButtonPrimary>
          "SEND"->React.string
        </Typography.ButtonPrimary>
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
            ~marginVertical=10.->dp,
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

    let onPressCancel = _e => {
      modal.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(ModalAction.closeModal)
      ->ignore;
    };

    <>
      <TouchableOpacity style=styles##button onPress>
        <Icon
          name=`add
          size=15.5
          color=Theme.colorDarkMediumEmphasis
          style=styles##icon
        />
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
             <AccountRowItem key={account.address} account />
           )
         ->React.array
       })}
    </Page>
    <SendButton />
  </>;
};
