open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

[@react.component]
let make = () => {
  let (network, _) = React.useContext(Network.context);
  let (account, _) = React.useContext(Account.context);

  let (selectedDelegate, setSelectedDelegate) = React.useState(() => "yo");
  <View style>
    <Picker
      selectedValue=selectedDelegate
      onValueChange={(value, _) => setSelectedDelegate(_ => value)}>
      <Picker.Item label="yo" value="yo" />
      <Picker.Item label="ga" value="ga" />
    </Picker>
    <Button
      onPress={_ =>
        API.Accounts.delegate(network, account, selectedDelegate)
        ->Future.flatMapOk(_ => API.Accounts.get())
        ->Future.get(result =>
            switch (result) {
            | Ok(_) => ()
            | Error(value) => Dialog.error(value)
            }
          )
      }
      title="Delegate"
    />
  </View>;
};
