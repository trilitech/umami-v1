open ReactNative;

let style = Style.(style(~padding=4.->dp, ()));

[@react.component]
let make = () => {
  let (name, setName) = React.useState(() => "bob");
  let (contracts, setContracts) = React.useState(() => "");

  React.useEffect1(
    () => {
      API.Accounts.get()->FutureEx.getOk(value => setContracts(_ => value));
      None;
    },
    [|setContracts|],
  );

  <View>
    <TextInput style onChangeText={text => setName(_ => text)} value=name />
    <Button
      onPress={
        _ =>
          name
          ->API.Accounts.post
          ->FutureEx.getOk(_ =>
              API.Accounts.get()
              ->FutureEx.getOk(value => setContracts(_ => value))
            )
      }
      title="Create"
    />
    <Text style> contracts->React.string </Text>
  </View>;
};