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

let sort = op =>
  Operation.(
    op->Belt.SortArray.stableSortInPlaceBy(
      ({timestamp: t1}, {timestamp: t2}) =>
      - Pervasives.compare(t1, t2)
    )
  );

[@react.component]
let make = () => {
  let operations = StoreContext.useOperations();
  let account = StoreContext.useAccount();
  let network = StoreContext.useNetwork();

  let (get, operationsRequest) = OperationApiRequest.useGetOperations();

  React.useEffect2(
    () => {
      get(network, account);
      None;
    },
    (network, account),
  );

  <View style=styles##container>
    <OperationsHeaderView />
    {switch (operationsRequest) {
     | Done(Ok(_)) =>
       <FlatList
         style=styles##list
         contentContainerStyle=styles##listContent
         data={operations->sort;
               operations}
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
