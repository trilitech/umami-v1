open ReactNative;

[@react.component]
let make = () => {
  let tokensRequest = StoreContext.Tokens.useLoad();

  <Page>
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
