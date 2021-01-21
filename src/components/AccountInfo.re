open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "alias": style(~height=20.->dp, ~marginBottom=4.->dp, ()),
      "balanceEmpty": style(~height=4.->dp, ()),
      "address": style(~height=18.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~account: Account.t,
      ~token: option(Token.t)=?,
      ~showBalance=true,
      ~showAlias=true,
    ) => {
  <View>
    {<Typography.Subtitle1 style=styles##alias>
       account.alias->React.string
     </Typography.Subtitle1>
     ->ReactUtils.onlyWhen(showAlias)}
    {showBalance
       ? <AccountInfoBalance address={account.address} ?token />
       : <View style=styles##balanceEmpty />}
    <Typography.Address style=styles##address>
      account.address->React.string
    </Typography.Address>
  </View>;
};
