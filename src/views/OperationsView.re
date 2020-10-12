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

  [@react.component]
  let make = (~operation: Operation.t) => {
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
    </View>;
  };
};

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

module Operations = API.Operations(API.TezosExplorer);

[@react.component]
let make = () => {
  let (network, _) = React.useContext(Network.context);
  let (account, _) = React.useContext(Account.context);
  let (injection, _) = React.useContext(Injection.context);
  let (operations: array(Operation.t), setOperations) =
    React.useState(() => [||]);

  React.useEffect4(
    () => {
      switch (injection) {
      | Pending(_) => ()
      | Done =>
        network
        ->Operations.get(account)
        ->FutureEx.getOk(value => setOperations(_ => value))
      };
      None;
    },
    (network, account, injection, setOperations),
  );

  <View>
    {operations
     ->Belt.Array.map(operation =>
         <OperationItem key={operation.id} operation />
       )
     ->React.array}
  </View>;
};
