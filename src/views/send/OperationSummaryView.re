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
  let make = (~account: Account.t, ~title) => {
    <>
      <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##title>
        title->React.string
      </Typography.Overline2>
      <Typography.Subtitle1 style=styles##subtitle>
        account.alias->React.string
      </Typography.Subtitle1>
      <Typography.Body1 colorStyle=`mediumEmphasis>
        account.address->React.string
      </Typography.Body1>
    </>;
  };
};

[@react.component]
let make = (~style=?, ~sender: Account.t, ~recipient: Account.t) => {
  <View ?style>
    <AccountInfo account=sender title="Sender account" />
    <View style=styles##iconContainer>
      <Icon name=`arrowDown size=50. color=Theme.colorDarkMediumEmphasis />
    </View>
    <AccountInfo account=recipient title="Recipient account" />
  </View>;
};
