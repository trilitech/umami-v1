open ReactNative;

module AddContactButton = {
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
          "ADD CONTACT"->React.string
        </Typography.ButtonSecondary>
      </TouchableOpacity>
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <ContactAddView onPressCancel handleAdd />
      </ModalAction>
    </>;
  };
};

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

[@react.component]
let make = () => {
  let (getAliases, aliasesRequest) = AliasApiRequest.useGet();
  let aliases = StoreContext.useAliases();

  React.useEffect0(() => {
    getAliases()->ignore;
    None;
  });

  let handleAdd = () => {
    getAliases(~loading=false, ())->ignore;
  };

  let handleDelete = handleAdd;

  <Page>
    <AddContactButton handleAdd />
    {switch (aliasesRequest) {
     | Done(Ok(_)) =>
       aliases
       ->Belt.Array.map(((alias, address)) => {
           let account: Account.t = {alias, address};
           account;
         })
       ->Belt.Array.mapWithIndex((index, account) =>
           <AddressBookRowItem
             key={account.address}
             account
             handleDelete
             zIndex={aliases->Belt.Array.size - index}
           />
         )
       ->React.array
     | Done(Error(error)) => <ErrorView error />
     | NotAsked
     | Loading => <LoadingView />
     }}
  </Page>;
};
