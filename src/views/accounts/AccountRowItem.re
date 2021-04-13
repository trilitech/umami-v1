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

  let balanceRequest = StoreContext.Balance.useLoad(account.address);

  let zeroTez =
    switch (balanceRequest->ApiRequest.getDoneOk) {
    | None => true
    | Some(balance) => balance == ProtocolXTZ.zero
    };

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
    {delegateRequest->ApiRequest.mapWithDefault(React.null, delegate => {
       <View style=styles##actionContainer>
         <DelegateButton
           zeroTez
           action={
             delegate->Option.mapWithDefault(
               Delegate.Create(Some(account)), delegate =>
               Delegate.Edit(account, delegate)
             )
           }
         />
       </View>
     })}
  </RowItem.Bordered>;
};
