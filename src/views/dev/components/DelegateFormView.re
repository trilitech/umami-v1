open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module Delegates = API.Delegates(API.TezosExplorer);

[@react.component]
let make = (~onSubmit) => {
  let (network, _) = React.useContext(NetworkState.context);

  let (source, setSource) = React.useState(() => "");
  let (selectedDelegateIndex, setSelectedDelegateIndex) =
    React.useState(() => 0);
  let (delegates, setDelegates) = React.useState(() => [||]);

  React.useEffect2(
    () => {
      Delegates.get(network)
      ->FutureEx.getOk(value => setDelegates(_ => value));
      None;
    },
    (network, setDelegates),
  );

  <View style>
    <TextInput
      onChangeText={text => setSource(_ => text)}
      placeholder="source"
      value=source
    />
    <Picker
      selectedValue=selectedDelegateIndex
      onValueChange={(value, _) => setSelectedDelegateIndex(_ => value)}>
      {React.array(
         delegates
         |> Array.mapi((index, delegate: API.Delegate.t) =>
              <Picker.Item label=delegate.name key=delegate.name value=index />
            ),
       )}
    </Picker>
    <Button
      onPress={_ => onSubmit(source, delegates[selectedDelegateIndex].address)}
      title="Delegate"
    />
  </View>;
};
