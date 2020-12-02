open ReactNative;

let styles =
  Style.(StyleSheet.create({"container": style(~marginBottom=10.->dp, ())}));

[@react.component]
let make = () => {
  let balanceTotal = StoreContext.Balance.useGetTotal();

  <View style=styles##container>
    <Typography.Overline2>
      "TOTAL BALANCE"->React.string
    </Typography.Overline2>
    <Typography.Headline2>
      {balanceTotal->Belt.Option.mapWithDefault(
         <ActivityIndicator
           animating=true
           size={ActivityIndicator_Size.exact(22.)}
           color=Colors.highIcon
         />,
         balanceTotal =>
         I18n.t#xtz_amount(balanceTotal->BusinessUtils.formatXTZ)
         ->React.string
       )}
    </Typography.Headline2>
  </View>;
};
