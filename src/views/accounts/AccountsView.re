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
        <AccountCreateView onPressCancel />
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
  <Page>
    {accounts->Belt.Option.mapWithDefault(<LoadingView />, accounts => {
       accounts->Belt.Map.String.size > 0
         ? <>
             <AddAccountButton />
             {accounts
              ->Belt.Map.String.valuesToArray
              ->Belt.SortArray.stableSortBy((a, b) => Pervasives.compare(a.alias, b.alias))
              ->Belt.Array.map(account =>
                  <AccountRowItem key={account.address} account />
                )
              ->React.array}
           </>
         : <View style=styles##scrim>
             <CreateAccountBigButton />
             <ImportAccountBigButton />
           </View>
     })}
  </Page>;
};
