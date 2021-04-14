open ReactNative;

module CellAddress =
  Table.MakeCell({
    let style =
      Style.(style(~flexBasis=180.->dp, ~flexGrow=1., ~flexShrink=1., ()));
  });

module CellAmount =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=146.->dp, ()));
  });

module CellDuration =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=114.->dp, ()));
  });

module CellReward =
  Table.MakeCell({
    let style = Style.(style(~flexBasis=160.->dp, ~flexGrow=1., ()));
  });

module CellAction =
  Table.MakeCell({
    let style =
      Style.(
        style(
          ~flexBasis=68.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          (),
        )
      );
  });

module DelegateActionButton = {
  [@react.component]
  let make = (~action, ~icon, ~tooltip=?) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _ => openAction();

    <>
      <IconButton ?tooltip icon size=30. onPress />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <DelegateView closeAction action />
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
  memo((~account: Account.t, ~delegateRequest) => {
    let aliases = StoreContext.Aliases.useGetAll();
    let balanceRequest = StoreContext.Balance.useLoad(account.address);
    let delegateInfoRequest =
      StoreContext.DelegateInfo.useLoad(account.address);

    let theme = ThemeContext.useTheme();

    switch ((delegateRequest: ApiRequest.t(_))) {
    | Done(Ok(Some(delegate)), _)
    | Loading(Some(Some(delegate))) =>
      <Table.Row>
        <CellAddress>
          <Typography.Body1 numberOfLines=1>
            account.alias->React.string
          </Typography.Body1>
        </CellAddress>
        <CellAmount>
          <Typography.Body1>
            {switch (delegateInfoRequest) {
             | Done(Ok(Some(delegateInfo)), _)
             | Loading(Some(Some(delegateInfo))) =>
               I18n.t#xtz_amount(
                 delegateInfo.initialBalance->ProtocolXTZ.toString,
               )
               ->React.string
             | Done(Ok(None), _)
             | Loading(Some(None))
             | Done(Error(_), _) => React.null
             | NotAsked
             | Loading(None) =>
               <ActivityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(19.)}
                 color={theme.colors.iconHighEmphasis}
               />
             }}
          </Typography.Body1>
        </CellAmount>
        <CellAmount>
          <Typography.Body1>
            {switch (balanceRequest) {
             | Done(Ok(balance), _)
             | Loading(Some(balance)) =>
               I18n.t#xtz_amount(balance->ProtocolXTZ.toString)->React.string
             | Done(Error(_error), _) => React.null
             | NotAsked
             | Loading(None) =>
               <ActivityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(19.)}
                 color={theme.colors.iconHighEmphasis}
               />
             }}
          </Typography.Body1>
        </CellAmount>
        <CellAddress>
          {delegate
           ->AliasHelpers.getAliasFromAddress(aliases)
           ->Option.mapWithDefault(
               <Typography.Address numberOfLines=1>
                 delegate->React.string
               </Typography.Address>,
               alias =>
               <Typography.Body1 numberOfLines=1>
                 alias->React.string
               </Typography.Body1>
             )}
        </CellAddress>
        <CellDuration>
          <Typography.Body1 numberOfLines=1>
            {switch (delegateInfoRequest) {
             | Done(Ok(Some(delegateInfo)), _)
             | Loading(Some(Some(delegateInfo))) =>
               Js.Date.make()
               ->DateFns.differenceInDays(delegateInfo.timestamp)
               ->(days => DateFns.formatDuration({days: days}))
               ->React.string
             | Done(Ok(None), _)
             | Loading(Some(None))
             | Done(Error(_), _) => React.null
             | NotAsked
             | Loading(None) =>
               <ActivityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(19.)}
                 color={theme.colors.iconHighEmphasis}
               />
             }}
          </Typography.Body1>
        </CellDuration>
        <CellReward>
          {switch (delegateInfoRequest) {
           | Done(Ok(Some({lastReward: Some(lastReward)})), _)
           | Loading(Some(Some({lastReward: Some(lastReward)}))) =>
             <Typography.Body1 colorStyle=`positive>
               {I18n.t#xtz_op_amount("+", lastReward->ProtocolXTZ.toString)
                ->React.string}
             </Typography.Body1>
           | Done(Ok(Some({lastReward: None})), _)
           | Loading(Some(Some({lastReward: None}))) =>
             <Typography.Body1> "---"->React.string </Typography.Body1>
           | Done(Ok(None), _)
           | Loading(Some(None))
           | Done(Error(_), _) => React.null
           | NotAsked
           | Loading(None) =>
             <Typography.Body1>
               <ActivityIndicator
                 animating=true
                 size={ActivityIndicator_Size.exact(19.)}
                 color={theme.colors.iconHighEmphasis}
               />
             </Typography.Body1>
           }}
        </CellReward>
        <CellAction>
          <DelegateActionButton
            action={Delegate.Edit(account, delegate)}
            tooltip=("delegate_edit", I18n.menu#delegate_edit)
            icon=Icons.Change.build
          />
          <DelegateActionButton
            action={Delegate.Delete(account, delegate)}
            tooltip=("delegate_delete", I18n.menu#delegate_delete)
            icon=Icons.Stop.build
          />
        </CellAction>
      </Table.Row>
    | Done(_)
    | NotAsked
    | Loading(_) => React.null
    };
  });
