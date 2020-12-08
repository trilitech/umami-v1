open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "balance": style(~height=19.->dp, ~marginBottom=2.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~token: option(Token.t)=?,
      ~balanceRequest: BalanceApiRequest.balanceApiRequest,
    ) => {
  <Typography.Subtitle3 style=styles##balance>
    {switch (balanceRequest) {
     | Done(Ok(balance)) =>
       I18n.t#amount(
         balance->BusinessUtils.formatXTZ,
         token->Belt.Option.mapWithDefault(I18n.t#xtz, token =>
           token.currency
         ),
       )
       ->React.string
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
