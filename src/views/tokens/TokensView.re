open ReactNative;

module AddTokenButton = {
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
      <View style=styles##button>
        <ButtonAction onPress text=I18n.btn#add_token icon=Icons.Add.build />
      </View>
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <TokenAddView cancel />
      </ModalAction>
    </>;
  };
};

let styles =
  Style.(StyleSheet.create({"list": style(~paddingTop=4.->dp, ())}));

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
    <View style=styles##list>
      {switch (tokensRequest) {
       | NotAsked
       | Loading(None) => <LoadingView />
       | Loading(Some(tokens))
       | Done(Ok(tokens), _) when tokens->Belt.Map.String.size == 0 =>
         <Table.Empty> I18n.t#empty_token->React.string </Table.Empty>
       | Loading(Some(tokens))
       | Done(Ok(tokens), _) =>
         tokens
         ->Belt.Map.String.valuesToArray
         ->Belt.Array.map(token => <TokenRowItem key={token.address} token />)
         ->React.array
       | Done(Error(error), _) => <ErrorView error />
       }}
    </View>
  </Page>;
};
