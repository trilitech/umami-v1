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
          I18n.btn#add_contact->React.string
        </Typography.ButtonSecondary>
      </TouchableOpacity>
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
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
