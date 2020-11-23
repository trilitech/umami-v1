open ReactNative;

module CellAddress =
  Table.MakeCell({
    let style =
      Style.(style(~flexBasis=180.->dp, ~flexGrow=1., ~flexShrink=1., ()));
    ();
  });

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##account == nextProps##account
  );

[@react.component]
let make =
  memo((~account: Account.t, ~zIndex) => {
    let delegateRequest = DelegateApiRequest.useGetDelegate(account.address);

    switch (delegateRequest) {
    | Done(Ok(Some(delegate))) =>
      <Table.Row zIndex>
        <CellAddress>
          <Typography.Body1 numberOfLines=1>
            account.alias->React.string
          </Typography.Body1>
        </CellAddress>
        <CellAddress>
          <Typography.Body1> delegate->React.string </Typography.Body1>
        </CellAddress>
      </Table.Row>
    | Done(_)
    | NotAsked
    | Loading => React.null
    };
  });
