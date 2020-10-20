open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~alignSelf=`flexStart,
          ~minWidth=360.->dp,
          ~height=54.->dp,
          ~paddingVertical=8.->dp,
          ~paddingHorizontal=12.->dp,
          ~flexDirection=`row,
          ~borderColor="rgba(255,255,255,0.6)",
          ~borderWidth=1.,
          ~borderRadius=5.,
          (),
        ),
      "inner": style(~justifyContent=`spaceBetween, ()),
      "text": style(~color="#FFF", ()),
    })
  );

[@react.component]
let make = () => {
  let account = StoreContext.useAccount();

  <View style=styles##container>
    {account->Belt.Option.mapWithDefault(<LoadingView />, account =>
       <View style=styles##inner>
         <Text style=styles##text> account.alias->React.string </Text>
         <Text style=styles##text> account.address->React.string </Text>
       </View>
     )}
  </View>;
};
