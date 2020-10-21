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
          ~minWidth=360.->dp,
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
          ~marginBottom=4.->dp,
          ~paddingLeft=15.->dp,
          ~borderColor="rgba(255,255,255,0.38)",
          ~borderBottomWidth=1.,
          ~zIndex=1,
          (),
        ),
      "headerText":
        style(
          ~color="rgba(255,255,255, 0.8)",
          ~fontSize=14.,
          ~fontWeight=`_300,
          (),
        ),
      "list": style(~flex=1., ~zIndex=1, ()),
      "listContent":
        style(
          ~flex=1.,
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
          <Text style=styles##headerText> "TYPE"->React.string </Text>
        </View>
        <View style=OperationRowItem.styles##cellAmount>
          <Text style=styles##headerText> "AMOUNT"->React.string </Text>
        </View>
        <View style=OperationRowItem.styles##cellFee>
          <Text style=styles##headerText> "FEE"->React.string </Text>
        </View>
        <View style=OperationRowItem.styles##cellAddress>
          <Text style=styles##headerText> "SENDER"->React.string </Text>
        </View>
        <View style=OperationRowItem.styles##cellAddress>
          <Text style=styles##headerText> "RECIPIENT"->React.string </Text>
        </View>
        <View style=OperationRowItem.styles##cellDate>
          <Text style=styles##headerText> "TIMESTAMP"->React.string </Text>
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
