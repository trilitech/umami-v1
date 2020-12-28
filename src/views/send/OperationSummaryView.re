open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=4.->dp, ()),
      "subtitle": style(~marginBottom=4.->dp, ()),
      "iconContainer": style(~padding=25.->dp, ()),
      "amountRow":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~justifyContent=`spaceBetween,
          (),
        ),
      "element": style(~marginTop=25.->dp, ()),
    })
  );

module Content = {
  [@react.component]
  let make = (~content) => {
    <View style=styles##element>
      {{
         content
         ->Belt.List.toArray
         ->Belt.Array.map(((property, value)) =>
             <View style=styles##amountRow>
               <Typography.Overline2>
                 property->React.string
               </Typography.Overline2>
               <Typography.Subtitle1>
                 value->React.string
               </Typography.Subtitle1>
             </View>
           );
       }
       ->React.array}
    </View>;
  };
};

module AccountInfo = {
  [@react.component]
  let make = (~address, ~title, ~style=?) => {
    let account: option(Account.t) =
      StoreContext.Accounts.useGetFromAddress(address);

    <View ?style>
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
    </View>;
  };
};

[@react.component]
let make = (~style=?, ~source, ~destination, ~content) => {
  <View ?style>
    <AccountInfo address={source->fst} title={source->snd} />
    {content->ReactUtils.hideNil(content => <Content content />)}
    <AccountInfo
      style=styles##element
      address={destination->fst}
      title={destination->snd}
    />
  </View>;
};
