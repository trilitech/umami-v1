open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=4.->dp, ()),
      "subtitle": style(~marginBottom=4.->dp, ()),
      "iconContainer": style(~padding=25.->dp, ()),
      "amount": style(~textAlign=`right, ()),
      "element": style(~marginTop=25.->dp, ()),
    })
  );

module Content = {
  [@react.component]
  let make = (~content) => {
    <View style=styles##element>
      {{
         content
         ->List.toArray
         ->Array.map(((property, values)) =>
             <View key=property style=FormStyles.amountRow>
               <Typography.Overline2>
                 property->React.string
               </Typography.Overline2>
               <View>
                 {values
                  ->List.mapWithIndex((i, value) =>
                      <Typography.Body1
                        key={i->Int.toString}
                        style=styles##amount
                        fontWeightStyle=`black>
                        value->React.string
                      </Typography.Body1>
                    )
                  ->List.toArray
                  ->React.array}
               </View>
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
      <Typography.Overline2 colorStyle=`mediumEmphasis style=styles##title>
        title->React.string
      </Typography.Overline2>
      {address
       ->AliasHelpers.getAliasFromAddress(aliases)
       ->ReactUtils.mapOpt(alias =>
           <Typography.Subtitle2 fontSize=16. style=styles##subtitle>
             alias->React.string
           </Typography.Subtitle2>
         )}
      <Typography.Address> address->React.string </Typography.Address>
    </View>;
  };
};

let buildDestinations = (destinations, showAmount) => {
  switch (destinations) {
  | `One(address, title) =>
    <AccountInfo style=styles##element address title />
  | `Many(recipients) =>
    <BatchView.Transactions recipients={recipients->List.reverse} showAmount />
  };
};

[@react.component]
let make = (~style=?, ~source, ~destinations, ~showCurrency, ~content) => {
  <View ?style>
    <AccountInfo address={source->fst} title={source->snd} />
    {content->ReactUtils.hideNil(content => <Content content />)}
    {buildDestinations(destinations, showCurrency)}
  </View>;
};
