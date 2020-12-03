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

module CellDuration =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=114.->dp, ()));
    ();
  });

module CellReward =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=160.->dp, ~flexGrow=1., ()));
    ();
  });

module CellAction =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=68.->dp, ~alignItems=`flexEnd, ()));
    ();
  });

module DelegateActionButton = {
  [@react.component]
  let make = (~action, ~text, ~icon, ~colorStyle=?) => {
    let modal = React.useRef(Js.Nullable.null);

    let (visibleModal, setVisibleModal) = React.useState(_ => false);
    let openAction = () => setVisibleModal(_ => true);
    let closeAction = () => setVisibleModal(_ => false);

    let onPress = _ => {
      openAction();
    };

    let onPressCancel = _e => {
      modal.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(ModalAction.closeModal)
      ->ignore;
    };

    <>
      <Menu.Item text icon onPress ?colorStyle />
      <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
        <DelegateView onPressCancel action />
      </ModalAction>
    </>;
  };
};

let memo = component =>
  React.memoCustomCompareProps(component, (prevPros, nextProps) =>
    prevPros##account == nextProps##account
  );

[@react.component]
let make =
  memo((~account: Account.t, ~zIndex) => {
    let aliases = StoreContext.Aliases.useGetAll();
    let balanceRequest = StoreContext.Balance.useLoad(account.address);
    let delegateRequest = StoreContext.Delegate.useLoad(account.address);
    let delegateInfoRequest =
      StoreContext.DelegateInfo.useLoad(account.address);

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
            {switch (delegateInfoRequest) {
             | Done(Ok(delegateInfo)) =>
               I18n.t#xtz_amount(
                 delegateInfo.initialBalance->BusinessUtils.formatXTZ,
               )
               ->React.string
             | Done(Error(_error)) => React.null
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
        <CellAmount>
          <Typography.Body1>
            {switch (balanceRequest) {
             | Done(Ok(balance)) =>
               I18n.t#xtz_amount(balance->BusinessUtils.formatXTZ)
               ->React.string
             | Done(Error(_error)) => React.null
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
          <Typography.Body1 numberOfLines=1>
            {delegate->AliasHelpers.getAliasFromAddress(aliases)->React.string}
          </Typography.Body1>
        </CellAddress>
        <CellDuration>
          <Typography.Body1 numberOfLines=1>
            {switch (delegateInfoRequest) {
             | Done(Ok(delegateInfo)) =>
               Js.Date.make()
               ->DateFns.differenceInDays(delegateInfo.timestamp)
               ->(days => DateFns.formatDuration({days: days}))
               ->React.string
             | Done(Error(_error)) => React.null
             | NotAsked
             | Loading =>
               <ActivityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(19.)}
                 color=Colors.highIcon
               />
             }}
          </Typography.Body1>
        </CellDuration>
        <CellReward>
          {switch (delegateInfoRequest) {
           | Done(Ok({lastReward: Some(lastReward)})) =>
             <Typography.Body1 colorStyle=`valid>
               {I18n.t#xtz_op_amount(
                  "+",
                  lastReward->BusinessUtils.formatMilliXTZ,
                )
                ->React.string}
             </Typography.Body1>
           | Done(Ok({lastReward: None})) =>
             <Typography.Body1> "---"->React.string </Typography.Body1>
           | Done(Error(_error)) => React.null
           | NotAsked
           | Loading =>
             <Typography.Body1>
               <ActivityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(19.)}
                 color=Colors.highIcon
               />
             </Typography.Body1>
           }}
        </CellReward>
        <CellAction>
          <Menu icon=Icons.More.build size=30.>
            <DelegateActionButton
              action={Delegate.Edit(account, delegate)}
              text=I18n.t#delegate_menu_edit
              icon=Icons.Change.build
            />
            <DelegateActionButton
              action={Delegate.Delete(account, delegate)}
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
