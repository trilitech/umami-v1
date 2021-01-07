open ReactNative;

module AddContactButton = {
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

    let cancel = _e => closeAction();

    <>
      <View style=styles##button>
        <ButtonAction onPress text=I18n.btn#add_contact icon=Icons.Add.build />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ContactAddView cancel />
      </ModalAction>
    </>;
  };
};

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

[@react.component]
let make = () => {
  let aliasesRequest = StoreContext.Aliases.useRequest();

  <Page>
    <AddContactButton />
    {switch (aliasesRequest) {
     | Done(Ok(aliases), _)
     | Loading(Some(aliases)) =>
       aliases
       ->Belt.Map.String.valuesToArray
       ->Belt.Array.mapWithIndex((index, account) =>
           <AddressBookRowItem
             key={account.address}
             account
             zIndex={aliases->Belt.Map.String.size - index}
           />
         )
       ->React.array
     | Done(Error(error), _) => <ErrorView error />
     | NotAsked
     | Loading(None) => <LoadingView />
     }}
  </Page>;
};
