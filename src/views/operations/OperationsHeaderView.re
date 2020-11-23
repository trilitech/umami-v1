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
          ~minWidth=440.->dp,
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
        <Typography.Overline3>
          I18n.t#operation_column_type->React.string
        </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellAmount>
        <Typography.Overline3>
          I18n.t#operation_column_amount->React.string
        </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellFee>
        <Typography.Overline3>
          I18n.t#operation_column_fee->React.string
        </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellAddress>
        <Typography.Overline3>
          I18n.t#operation_column_sender->React.string
        </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellAddress>
        <Typography.Overline3>
          I18n.t#operation_column_recipient->React.string
        </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellDate>
        <Typography.Overline3>
          I18n.t#operation_column_timestamp->React.string
        </Typography.Overline3>
      </View>
      <View style=OperationRowItem.styles##cellStatus>
        <Typography.Overline3>
          I18n.t#operation_column_status->React.string
        </Typography.Overline3>
      </View>
    </View>
  </View>;
};
