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
          ~paddingBottom=LayoutConst.pagePaddingVertical->dp,
          ~paddingHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

let renderItem =
    (
      currentLevel,
      renderItemProps: VirtualizedList.renderItemProps(Operation.Read.t),
    ) => {
  let operation = renderItemProps.item;
  <OperationRowItem operation currentLevel />;
};

let keyExtractor = (operation: Operation.Read.t, _i) => {
  operation.id;
};

let _ListEmptyComponent = () => <EmptyView text="No operations" />;

let sort = op =>
  Operation.Read.(
    op->SortArray.stableSortBy(({timestamp: t1}, {timestamp: t2}) =>
      - Pervasives.compare(Js.Date.getTime(t1), Js.Date.getTime(t2))
    )
  );

[@react.component]
let make = () => {
  let account = StoreContext.SelectedAccount.useGet();

  let operationsRequest =
    StoreContext.Operations.useLoad(
      ~address=account->Option.map(account => account.address),
      (),
    );

  <View style=styles##container>
    <OperationsHeaderView />
    {ApiRequest.(
       switch (operationsRequest) {
       | Done(Ok(operations), _)
       | Loading(Some(operations)) =>
         <FlatList
           style=styles##list
           contentContainerStyle=styles##listContent
           data={operations->fst->sort}
           initialNumToRender=20
           keyExtractor
           renderItem={renderItem(operations->snd)}
           _ListEmptyComponent
         />
       | Done(Error(error), _) => error->React.string
       | NotAsked
       | Loading(None) => <LoadingView />
       }
     )}
  </View>;
};
