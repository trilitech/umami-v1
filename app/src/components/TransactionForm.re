open ReactNative;

let style = Style.(style(~padding=4.->dp, ()));

[@react.component]
let make = (~onSubmit) => {
  let (amount, setAmount) = React.useState(() => 1.0);
  let (destination, setDestination) = React.useState(() => "bob");

  <View>
    <TextInput
      style
      onChangeText={
        text =>
          text
          ->Js.Float.fromString
          ->(x => Js.Float.isNaN(x) ? amount : x)
          ->(x => setAmount(_ => x))
      }
      value={Js.Float.toString(amount)}
    />
    <TextInput
      style
      onChangeText={text => setDestination(_ => text)}
      value=destination
    />
    <Button onPress={_ => onSubmit(amount, destination)} title="Send" />
  </View>;
};