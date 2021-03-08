open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "inner": style(~marginRight=10.->dp, ~marginLeft=14.->dp, ()),
      "actionButtons":
        style(
          ~alignSelf=`flexEnd,
          ~flexDirection=`row,
          ~flex=1.,
          ~marginBottom=6.->dp,
          (),
        ),
      "button": style(~marginRight=4.->dp, ()),
      "actionContainer": style(~marginRight=24.->dp, ()),
    })
  );

[@react.component]
let make = (~account: Account.t, ~token: option(Token.t)=?) => {
  let delegateRequest = StoreContext.Delegate.useLoad(account.address);
  let addToast = LogsContext.useToast();

  <RowItem.Bordered height=90.>
    <View style=styles##inner> <AccountInfo account ?token /> </View>
    <View style=styles##actionButtons>
      <ClipboardButton
        copied=I18n.log#address
        addToast
        data={account.address}
        style=styles##button
      />
      <QrButton account style=styles##button />
    </View>
    {delegateRequest->ApiRequest.mapOkWithDefault(React.null, delegate => {
       <View style=styles##actionContainer>
         <DelegateButton account disabled={delegate->Option.isSome} />
       </View>
     })}
  </RowItem.Bordered>;
};
