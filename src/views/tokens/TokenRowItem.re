open ReactNative;

module CellName =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=132.->dp, ()));
    ();
  });

module CellSymbol =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=90.->dp, ()));
    ();
  });

module CellAddress =
  Table.MakeCell({
    let style =
      Style.(style(~flexBasis=180.->dp, ~flexGrow=1., ~flexShrink=1., ()));
    ();
  });

[@react.component]
let make = (~token: Token.t) => {
  <Table.Row>
    <CellName>
      <Typography.Body1 numberOfLines=1>
        token.alias->React.string
      </Typography.Body1>
    </CellName>
    <CellSymbol>
      <Typography.Body1 numberOfLines=1>
        token.symbol->React.string
      </Typography.Body1>
    </CellSymbol>
    <CellAddress>
      <Typography.Body1 numberOfLines=1>
        token.address->React.string
      </Typography.Body1>
    </CellAddress>
  </Table.Row>;
};
