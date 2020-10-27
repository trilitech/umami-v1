open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flex=1., ()),
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
      "list": style(~flex=1., ~zIndex=1, ()),
      "listContent":
        style(
          ~flex=1.,
          ~paddingTop=4.->dp,
          ~paddingBottom=Theme.pagePaddingVertical->dp,
          ~paddingHorizontal=Theme.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

let renderItem =
    (renderItemProps: VirtualizedList.renderItemProps(Operation.t)) => {
  let operation = renderItemProps.item;
  <OperationRowItem operation />;
};

let keyExtractor = (operation: Operation.t, _i) => {
  operation.id;
};

let _ListEmptyComponent = () => <EmptyView text="No operations" />;

[@react.component]
let make = () => {
  let operationsRequest =
    OperationApiRequest.useGetOperations(~limit=100, ());

  <View style=styles##container>
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
    </View>
    {switch (operationsRequest) {
     | Done(Ok(operations)) =>
       <FlatList
         style=styles##list
         contentContainerStyle=styles##listContent
         data=operations
         initialNumToRender=20
         keyExtractor
         renderItem
         _ListEmptyComponent
       />
     | Done(Error(error)) => error->React.string
     | NotAsked
     | Loading => <LoadingView />
     }}
  </View>;
};
