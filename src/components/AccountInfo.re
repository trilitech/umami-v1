open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~height=82.->dp, ()),
      "alias": style(~height=19.->dp, ~marginBottom=2.->dp, ()),
      "balance": style(~height=19.->dp, ~marginBottom=5.->dp, ()),
      "balanceEmpty": style(~height=2.->dp, ()),
      "addressLabel": style(~height=16.->dp, ~marginBottom=2.->dp, ()),
      "address": style(~height=19.->dp, ()),
    })
  );

let balance = (request: BalanceApiRequest.balanceApiRequest) => {
  <Typography.Subtitle3 style=styles##balance>
    {switch (request) {
     | Done(Ok(balance)) => balance->React.string
     | Done(Error(error)) => error->React.string
     | NotAsked
     | Loading =>
       <ActivityIndicator
         animating=true
         size={ActivityIndicator_Size.exact(19.)}
         color=Colors.highIcon
       />
     }}
  </Typography.Subtitle3>;
};

[@react.component]
let make =
    (
      ~account: Account.t,
      ~balanceRequest: option(BalanceApiRequest.balanceApiRequest)=?,
    ) => {
  <View style=styles##container>
    <Typography.Subtitle1 style=styles##alias>
      account.alias->React.string
    </Typography.Subtitle1>
    {balanceRequest->Belt.Option.mapWithDefault(
       <View style=styles##balanceEmpty />,
       balance,
     )}
    <Typography.Subtitle4 style=styles##addressLabel>
      "Address"->React.string
    </Typography.Subtitle4>
    <Typography.Body3 style=styles##address>
      account.address->React.string
    </Typography.Body3>
  </View>;
};
