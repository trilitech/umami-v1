open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=4.->dp, ()),
      "subtitle": style(~marginBottom=4.->dp, ()),
      "iconContainer": style(~padding=25.->dp, ()),
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
             <View key=property style=FormStyles.amountRow>
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
    let aliases = StoreContext.Aliases.useGetAll();

    <View ?style>
      <Typography.Overline1 colorStyle=`mediumEmphasis style=styles##title>
        title->React.string
      </Typography.Overline1>
      {address
       ->AliasHelpers.getAliasFromAddress(aliases)
       ->ReactUtils.mapOpt(alias =>
           <Typography.Subtitle1 style=styles##subtitle>
             alias->React.string
           </Typography.Subtitle1>
         )}
      <Typography.Address fontSize=16.>
        address->React.string
      </Typography.Address>
    </View>;
  };
};

let buildDestinations = destinations => {
  switch (destinations) {
  | `One(address, title) =>
    <AccountInfo style=styles##element address title />
  | `Many(recipients) => <BatchView.Transactions recipients />
  };
};

[@react.component]
let make = (~style=?, ~source, ~destinations, ~content) => {
  <View ?style>
    <AccountInfo address={source->fst} title={source->snd} />
    {content->ReactUtils.hideNil(content => <Content content />)}
    {buildDestinations(destinations)}
  </View>;
};
