open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flex=1., ()),
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
    <OperationsHeaderView />
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
