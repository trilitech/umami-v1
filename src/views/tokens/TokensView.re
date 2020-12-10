open ReactNative;

module AddTokenButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~alignSelf=`flexStart,
            ~marginBottom=18.->dp,
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
          I18n.btn#add_token->React.string
        </Typography.ButtonSecondary>
      </TouchableOpacity>
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <TokenAddView cancel />
      </ModalAction>
    </>;
  };
};

[@react.component]
let make = () => {
  let tokensRequest = StoreContext.Tokens.useLoad();

  <Page>
    <AddTokenButton />
    <Table.Head>
      <TokenRowItem.CellName>
        <Typography.Overline3>
          I18n.t#token_column_name->React.string
        </Typography.Overline3>
      </TokenRowItem.CellName>
      <TokenRowItem.CellSymbol>
        <Typography.Overline3>
          I18n.t#token_column_symbol->React.string
        </Typography.Overline3>
      </TokenRowItem.CellSymbol>
      <TokenRowItem.CellAddress>
        <Typography.Overline3>
          I18n.t#token_column_address->React.string
        </Typography.Overline3>
      </TokenRowItem.CellAddress>
    </Table.Head>
    {switch (tokensRequest) {
     | Done(Ok(tokens)) =>
       tokens
       ->Belt.Map.String.valuesToArray
       ->Belt.Array.map(token => <TokenRowItem key={token.address} token />)
       ->React.array
     | Done(Error(error)) => <ErrorView error />
     | NotAsked
     | Loading => <LoadingView />
     }}
  </Page>;
};
