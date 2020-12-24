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
      <Typography.Overline1 colorStyle=`mediumEmphasis style=styles##title>
        title->React.string
      </Typography.Overline1>
      {account->ReactUtils.mapOpt(account =>
         <Typography.Subtitle1 style=styles##subtitle>
           account.alias->React.string
         </Typography.Subtitle1>
       )}
      <Typography.Address fontSize=16.>
        address->React.string
      </Typography.Address>
    </>;
  };
};

[@react.component]
let make = (~style=?, ~source, ~destination) => {
  let theme = ThemeContext.useTheme();
  <View ?style>
    <AccountInfo address={source->fst} title={source->snd} />
    <View style=styles##iconContainer>
      <Icons.ArrowDown size=50. color={theme.colors.iconMediumEmphasis} />
    </View>
    <AccountInfo address={destination->fst} title={destination->snd} />
  </View>;
};
