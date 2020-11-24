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

module CellAction =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=68.->dp, ~alignItems=`flexEnd, ()));
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
        <CellAction>
          <Menu icon=Icons.More.build size=30.>
            <Menu.Item
              text=I18n.t#delegate_menu_edit
              icon=Icons.Change.build
            />
            <Menu.Item
              text=I18n.t#delegate_menu_delete
              colorStyle=`error
              icon=Icons.Close.build
            />
          </Menu>
        </CellAction>
      </Table.Row>
    | Done(_)
    | NotAsked
    | Loading => React.null
    };
  });
