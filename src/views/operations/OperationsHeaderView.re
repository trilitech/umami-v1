open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "header":
        style(
          ~marginHorizontal=Theme.pagePaddingHorizontal->dp,
          ~marginTop=Theme.pagePaddingVertical->dp,
          ~zIndex=2,
          (),
        ),
      "selector":
        style(
          ~alignSelf=`flexStart,
          ~minWidth=420.->dp,
          ~marginTop=0.->dp,
          ~marginBottom=30.->dp,
          ~zIndex=2,
          (),
        ),
      "thead":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~height=30.->dp,
          ~paddingLeft=22.->dp,
          ~borderColor="rgba(255,255,255,0.38)",
          ~borderBottomWidth=1.,
          ~zIndex=1,
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  <View style=styles##header>
    <AccountSelector style=styles##selector />
    <View style=styles##thead>
      <View style=OperationRowItem.styles##cellType>
        <Typography.Overline3> "TYPE"->React.string </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellAmount>
        <Typography.Overline3> "AMOUNT"->React.string </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellFee>
        <Typography.Overline3> "FEE"->React.string </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellAddress>
        <Typography.Overline3> "SENDER"->React.string </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellAddress>
        <Typography.Overline3>
          "RECIPIENT"->React.string
        </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellDate>
        <Typography.Overline3>
          "TIMESTAMP"->React.string
        </Typography.Overline3>
      </View>
    </View>
  </View>;
};
