open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~height=62.->dp, ()),
      "alias": style(~height=19.->dp, ~marginBottom=2.->dp, ()),
      "balanceEmpty": style(~height=2.->dp, ()),
      "address": style(~height=19.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~account: Account.t,
      ~token: option(Token.t)=?,
      ~balanceRequest: option(BalanceApiRequest.balanceApiRequest)=?,
      ~showAlias=true,
    ) => {
  <View style=styles##container>
    {<Typography.Subtitle1 style=styles##alias>
       account.alias->React.string
     </Typography.Subtitle1>
     ->ReactUtils.onlyWhen(showAlias)}
    {balanceRequest->Belt.Option.mapWithDefault(
       <View style=styles##balanceEmpty />, balanceRequest =>
       <AccountInfoBalance balanceRequest ?token />
     )}
    <Typography.Body3 style=styles##address>
      account.address->React.string
    </Typography.Body3>
  </View>;
};
