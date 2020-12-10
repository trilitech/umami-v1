open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=4.->dp, ()),
      "subtitle": style(~marginBottom=4.->dp, ()),
      "iconContainer": style(~padding=25.->dp, ()),
    })
  );

module AccountInfo = {
  [@react.component]
  let make = (~address, ~title) => {
    let account: option(Account.t) =
      StoreContext.Accounts.useGetFromAddress(address);

    <>
      <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##title>
        title->React.string
      </Typography.Overline2>
      {account->ReactUtils.mapOpt(account =>
         <Typography.Subtitle1 style=styles##subtitle>
           account.alias->React.string
         </Typography.Subtitle1>
       )}
      <Typography.Body1 colorStyle=`mediumEmphasis>
        address->React.string
      </Typography.Body1>
    </>;
  };
};

[@react.component]
let make = (~style=?, ~operation: SendForm.operation) => {
  <View ?style>
    {switch (operation) {
     | InjectionOperation(Transaction({source}))
     | TokensOperation({action: Transfer({source})}, _) =>
       <AccountInfo address=source title=I18n.title#sender_account />
     | InjectionOperation(Delegation({source})) =>
       <AccountInfo address=source title=I18n.title#delegated_account />
     | _ => React.null
     }}
    <View style=styles##iconContainer>
      <Icons.ArrowDown size=50. color=Theme.colorDarkMediumEmphasis />
    </View>
    {switch (operation) {
     | InjectionOperation(Transaction({destination}))
     | TokensOperation({action: Transfer({destination})}, _) =>
       <AccountInfo address=destination title=I18n.title#recipient_account />
     | InjectionOperation(Delegation({delegate})) =>
       <AccountInfo address=delegate title=I18n.title#baker_account />
     | _ => React.null
     }}
  </View>;
};
