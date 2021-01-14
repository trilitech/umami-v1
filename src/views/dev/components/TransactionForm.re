open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

[@react.component]
let make = (~onSubmit) => {
  let (amount, setAmount) = React.useState(() => ProtocolXTZ.zero);
  let (source, setSource) = React.useState(() => "");
  let (destination, setDestination) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text =>
        text
        ->ProtocolXTZ.fromString
        ->(x => x->Belt.Option.getWithDefault(amount))
        ->(x => setAmount(_ => x))
      }
      placeholder="amount"
      value={amount == ProtocolXTZ.zero ? "" : ProtocolXTZ.toString(amount)}
    />
    <TextInput
      onChangeText={text => setSource(_ => text)}
      placeholder="source"
      value=source
    />
    <TextInput
      onChangeText={text => setDestination(_ => text)}
      placeholder="destination"
      value=destination
    />
    <Button
      onPress={_ => onSubmit(source, amount, destination)}
      title="Send"
    />
  </View>;
};
