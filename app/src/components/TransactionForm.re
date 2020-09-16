open ReactNative;

let style = Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

[@react.component]
let make = (~onSubmit) => {
  let (amount, setAmount) = React.useState(() => 0.0);
  let (source, setSource) = React.useState(() => "");
  let (destination, setDestination) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={
        text =>
          text
          ->Js.Float.fromString
          ->(x => Js.Float.isNaN(x) ? amount : x)
          ->(x => setAmount(_ => x))
      }
      placeholder="amount"
      value={amount == 0.0 ? "" : Js.Float.toString(amount)}
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
    <Button onPress={_ => onSubmit(source, amount, destination)} title="Send" />
  </View>
};