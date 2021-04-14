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
  let make = (~chain=?) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _ => openAction();

    let tooltip =
      chain == None
        ? Some((
            "add_token_button",
            I18n.form_input_error#chain_not_connected,
          ))
        : None;

    <>
      <View style=styles##button>
        <ButtonAction
          disabled={chain == None}
          ?tooltip
          onPress
          text=I18n.btn#add_token
          icon=Icons.Add.build
        />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <TokenAddView chain={chain->Option.getWithDefault("")} closeAction />
      </ModalAction>
    </>;
  };
};

let styles =
  Style.(StyleSheet.create({"list": style(~paddingTop=4.->dp, ())}));

[@react.component]
let make = () => {
  let tokensRequest = StoreContext.Tokens.useRequest();
  let apiVersion: option(Network.apiVersion) = StoreContext.useApiVersion();

  <Page>
    <AddTokenButton chain=?{apiVersion->Option.map(v => v.chain)} />
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
       | Done(Ok(tokens), _) when tokens->Map.String.size == 0 =>
         <Table.Empty> I18n.t#empty_token->React.string </Table.Empty>
       | Loading(Some(tokens))
       | Done(Ok(tokens), _) =>
         tokens
         ->Map.String.valuesToArray
         ->Array.map(token => <TokenRowItem key={token.address} token />)
         ->React.array
       | Done(Error(error), _) => <ErrorView error />
       }}
    </View>
  </Page>;
};
