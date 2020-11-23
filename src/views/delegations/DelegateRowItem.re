open ReactNative;

module CellAddress =
  Table.MakeCell({
    let style =
      Style.(style(~flexBasis=180.->dp, ~flexGrow=1., ~flexShrink=1., ()));
    ();
  });

module CellAmount =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=146.->dp, ()));
    ();
  });

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##account == nextProps##account
  );

[@react.component]
let make =
  memo((~account: Account.t, ~zIndex) => {
    let balanceRequest = BalanceApiRequest.useLoad(account.address);
    let delegateRequest = DelegateApiRequest.useGetDelegate(account.address);

    switch (delegateRequest) {
    | Done(Ok(Some(delegate))) =>
      <Table.Row zIndex>
        <CellAddress>
          <Typography.Body1 numberOfLines=1>
            account.alias->React.string
          </Typography.Body1>
        </CellAddress>
        <CellAmount>
          <Typography.Body1>
            {switch (balanceRequest) {
             | Done(Ok(balance)) =>
               (balance->BusinessUtils.formatXTZ ++ " XTZ")->React.string
             | Done(Error(error)) => error->React.string
             | NotAsked
             | Loading =>
               <ActivityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(19.)}
                 color=Colors.highIcon
               />
             }}
          </Typography.Body1>
        </CellAmount>
        <CellAddress>
          <Typography.Body1> delegate->React.string </Typography.Body1>
        </CellAddress>
      </Table.Row>
    | Done(_)
    | NotAsked
    | Loading => React.null
    };
  });
