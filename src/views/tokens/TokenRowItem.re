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

module CellAction =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=68.->dp, ~alignItems=`flexEnd, ()));
    ();
  });

module TokenDeleteButton = {
  [@react.component]
  let make = (~token: Token.t) => {
    let (tokenRequest, deleteToken) = StoreContext.Tokens.useDelete();

    let onPressConfirmDelete = _e => {
      deleteToken(token)->ignore;
    };

    <DeleteButton
      buttonText=I18n.btn#delete_token
      modalTitle=I18n.title#delete_token
      onPressConfirmDelete
      request=tokenRequest
    />;
  };
};

[@react.component]
let make = (~token: Token.t, ~zIndex) => {
  <Table.Row zIndex>
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
      <Typography.Address numberOfLines=1>
        token.address->React.string
      </Typography.Address>
    </CellAddress>
    <CellAction>
      <Menu icon=Icons.More.build> <TokenDeleteButton token /> </Menu>
    </CellAction>
  </Table.Row>;
};
