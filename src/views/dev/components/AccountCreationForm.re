open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module AccountsAPI = API.Accounts(API.TezosClient);

[@react.component]
let make = () => {
  let (_, setAccounts) = React.useContext(AccountsState.context);
  let settings = SdkContext.useSettings();
  let (name, setName) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setName(_ => text)}
      placeholder="alias"
      value=name
    />
    <Button
      onPress={_ =>
        name
        ->AccountsAPI.create(~settings)
        ->Future.flatMapOk(_ => AccountsAPI.get(~settings))
        ->Future.get(result =>
            switch (result) {
            | Ok(value) => setAccounts(value)
            | Error(value) => Dialog.error(value)
            }
          )
      }
      title="Create"
    />
  </View>;
};
