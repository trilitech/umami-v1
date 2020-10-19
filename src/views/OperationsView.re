open ReactNative;

module OperationItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~height=36.->dp, ~flexDirection=`row, ()),
        "inner":
          style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
        "border":
          style(
            ~backgroundColor="#8D9093",
            ~width=4.->dp,
            ~marginRight=11.->dp,
            ~borderTopRightRadius=4.,
            ~borderBottomRightRadius=4.,
            (),
          ),
        "cell": style(~flexGrow=1., ()),
        "text":
          style(
            ~color="rgba(255,255,255, 0.87)",
            ~fontSize=16.,
            ~fontWeight=`_400,
            (),
          ),
      })
    );

  let memo = component =>
    React.memoCustomCompareProps(component, (prevPros, nextProps) =>
      prevPros##operation == nextProps##operation
    );

  [@react.component]
  let make =
    memo((~operation: Operation.t) => {
      <View style=styles##container>
        <View style=styles##border />
        <View style=styles##inner>
          <View style=styles##cell>
            <Text style=styles##text> operation.id->React.string </Text>
          </View>
          <View style=styles##cell>
            <Text style=styles##text> operation.level->React.string </Text>
          </View>
          <View style=styles##cell>
            <Text style=styles##text>
              {operation.timestamp->Js.Date.toISOString->React.string}
            </Text>
          </View>
          <View style=styles##cell>
            <Text style=styles##text> operation.block->React.string </Text>
          </View>
        </View>
      </View>
    });
};

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

let renderItem =
    (renderItemProps: VirtualizedList.renderItemProps(Operation.t)) => {
  let operation = renderItemProps.item;
  <OperationItem operation />;
};

let keyExtractor = (operation: Operation.t, _i) => {
  operation.id;
};

[@react.component]
let make = () => {
  let operationsRequest = ApiRequest.useGetOperations(~limit=100, ());

  switch (operationsRequest) {
  | Done(Ok(operations)) =>
    <FlatList
      style=Page.styles##scroll
      contentContainerStyle=Page.styles##scrollContent
      data=operations
      keyExtractor
      renderItem
      initialNumToRender=20
    />
  | Done(Error(error)) => error->React.string
  | NotAsked
  | Loading => <LoadingView />
  };
};
