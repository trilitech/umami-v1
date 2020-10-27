open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "inner":
        style(
          ~marginLeft=14.->dp,
          ~alignSelf=`stretch,
          ~justifyContent=`spaceBetween,
          (),
        ),
    })
  );

[@react.component]
let make = (~account: Account.t) => {
  let balanceRequest = BalanceApiRequest.useBalance(account.address);

  <RowItem height=94.>
    {_ => {
       <View style=styles##inner>
         <Typography.Subtitle1>
           account.alias->React.string
         </Typography.Subtitle1>
         <Typography.Subtitle3>
           {switch (balanceRequest) {
            | Done(Ok(balance)) => balance->React.string
            | Done(Error(error)) => error->React.string
            | NotAsked
            | Loading =>
              <ActivityIndicator
                animating=true
                size={ActivityIndicator_Size.exact(17.)}
                color=Colors.highIcon
              />
            }}
         </Typography.Subtitle3>
         <Typography.Subtitle4> "Address"->React.string </Typography.Subtitle4>
         <Typography.Body3> account.address->React.string </Typography.Body3>
       </View>;
     }}
  </RowItem>;
};
