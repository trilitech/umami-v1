open ReactNative;

module AccountItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~height=84.->dp, ~flexDirection=`row, ()),
        "inner": style(~justifyContent=`spaceBetween, ()),
        "border":
          style(
            ~backgroundColor="#8D9093",
            ~width=4.->dp,
            ~marginRight=11.->dp,
            ~borderTopRightRadius=4.,
            ~borderBottomRightRadius=4.,
            (),
          ),
        "title":
          style(
            ~color="rgba(255,255,255, 0.8)",
            ~fontSize=14.,
            ~fontWeight=`bold,
            (),
          ),
        "balance":
          style(
            ~color="rgba(255,255,255, 0.87)",
            ~fontSize=14.,
            ~fontWeight=`bold,
            (),
          ),
        "currency":
          style(
            ~color="rgba(255,255,255, 1)",
            ~fontSize=14.,
            ~fontWeight=`normal,
            (),
          ),
        "label":
          style(
            ~color="rgba(255,255,255, 0.6)",
            ~fontSize=12.,
            ~fontWeight=`_700,
            (),
          ),
        "address":
          style(
            ~color="rgba(255,255,255, 0.87)",
            ~fontSize=14.,
            ~fontWeight=`_400,
            (),
          ),
      })
    );

  [@react.component]
  let make = () => {
    <View style=styles##container>
      <View style=styles##border />
      <View style=styles##inner>
        <Text style=styles##title> "Account 1"->React.string </Text>
        <Text style=styles##balance>
          "375.033287"->React.string
          <Text style=styles##currency> {js| ꜩ|js}->React.string </Text>
        </Text>
        <Text style=styles##label> "Address"->React.string </Text>
        <Text style=styles##address>
          "tz1c5wM9826YcUNQ8a17"->React.string
        </Text>
      </View>
    </View>;
  };
};

let styles = Style.(StyleSheet.create({}));

[@react.component]
let make = () => {
  let (href, onPress) = Routes.useHrefAndOnPress(Routes.Dev);

  <View> <AccountItem /> </View>;
};
