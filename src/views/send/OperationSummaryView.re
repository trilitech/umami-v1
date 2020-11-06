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
      StoreContext.useAccountFromAddress(address);

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
let make = (~style=?, ~transaction: Injection.transaction) => {
  <View ?style>
    <AccountInfo address={transaction.source} title="Sender account" />
    <View style=styles##iconContainer>
      <Icon name=`arrowDown size=50. color=Theme.colorDarkMediumEmphasis />
    </View>
    <AccountInfo address={transaction.destination} title="Recipient account" />
  </View>;
};
