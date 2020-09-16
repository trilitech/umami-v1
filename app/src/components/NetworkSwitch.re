open ReactNative;

let style =
  Style.(
    style(
      ~flex=1.,
      ~justifyContent=`flexEnd,
      ~flexDirection=`row,
      ~padding=4.->dp,
      (),
    )
  );

[@react.component]
let make = () => {
  let (network, setNetwork) = React.useContext(Network.context);
  <View style>
    <Text> "testnet  "->React.string </Text>
    <Switch
      onValueChange={value => setNetwork(value ? Network.Main : Network.Test)}
      value={network == Main}
    />
    <Text> "  mainnet"->React.string </Text>
  </View>;
};
