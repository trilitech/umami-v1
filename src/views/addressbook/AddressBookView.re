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

    <>
      <View style=styles##button>
        <ButtonAction onPress text=I18n.btn#add_contact icon=Icons.Add.build />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ContactFormView action=Create closeAction />
      </ModalAction>
    </>;
  };
};

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

[@react.component]
let make = () => {
  let aliasesRequest = StoreContext.Aliases.useRequestExceptAccounts();

  <Page>
    <AddContactButton />
    {switch (aliasesRequest) {
     | Done(Ok(aliases), _)
     | Loading(Some(aliases)) =>
       aliases->Map.String.size === 0
         ? <Table.Empty> I18n.t#empty_address_book->React.string </Table.Empty>
         : aliases
           ->Map.String.valuesToArray
           ->SortArray.stableSortBy((a, b) =>
               Js.String.localeCompare(b.alias, a.alias)->int_of_float
             )
           ->Array.mapWithIndex((index, account) =>
               <AddressBookRowItem
                 key={account.address}
                 account
                 zIndex={aliases->Map.String.size - index}
               />
             )
           ->React.array
     | Done(Error(error), _) => <ErrorView error />
     | NotAsked
     | Loading(None) => <LoadingView />
     }}
  </Page>;
};
