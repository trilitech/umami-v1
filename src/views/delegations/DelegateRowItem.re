open ReactNative;

let baseCellStyle = Style.(style(~flexShrink=0., ~marginRight=24.->dp, ()));
let styles =
  Style.(
    StyleSheet.create({
      "borderSpacer": style(~width=20.->dp, ()),
      "cellAccount":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=120.->dp, ~flexGrow=1., ~flexShrink=1., ()),
        |]),
      "cellBaker":
        StyleSheet.flatten([|
          baseCellStyle,
          style(~flexBasis=120.->dp, ~flexGrow=1., ~flexShrink=1., ()),
        |]),
    })
  );

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
      <RowItem.Bordered height=48. style={Style.style(~zIndex, ())}>
        <View style=styles##borderSpacer />
        <View style=styles##cellAccount>
          <Typography.Body1 numberOfLines=1>
            account.alias->React.string
          </Typography.Body1>
        </View>
        <View style=styles##cellBaker>
          <Typography.Body1> delegate->React.string </Typography.Body1>
        </View>
      </RowItem.Bordered>
    | Done(_)
    | NotAsked
    | Loading => React.null
    };
  });
