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
    })
  );

[@react.component]
let make = () => {
  <View style=styles##header>
    <AccountSelector style=styles##selector />
    <Table.Head>
      <OperationRowItem.CellType>
        <Typography.Overline3>
          I18n.t#operation_column_type->React.string
        </Typography.Overline3>
      </OperationRowItem.CellType>
      <OperationRowItem.CellAmount>
        <Typography.Overline3>
          I18n.t#operation_column_amount->React.string
        </Typography.Overline3>
      </OperationRowItem.CellAmount>
      <OperationRowItem.CellFee>
        <Typography.Overline3>
          I18n.t#operation_column_fee->React.string
        </Typography.Overline3>
      </OperationRowItem.CellFee>
      <OperationRowItem.CellAddress>
        <Typography.Overline3>
          I18n.t#operation_column_sender->React.string
        </Typography.Overline3>
      </OperationRowItem.CellAddress>
      <OperationRowItem.CellAddress>
        <Typography.Overline3>
          I18n.t#operation_column_recipient->React.string
        </Typography.Overline3>
      </OperationRowItem.CellAddress>
      <OperationRowItem.CellDate>
        <Typography.Overline3>
          I18n.t#operation_column_timestamp->React.string
        </Typography.Overline3>
      </OperationRowItem.CellDate>
      <OperationRowItem.CellStatus>
        <Typography.Overline3>
          I18n.t#operation_column_status->React.string
        </Typography.Overline3>
      </OperationRowItem.CellStatus>
    </Table.Head>
  </View>;
};
