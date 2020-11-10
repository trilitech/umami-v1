open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module AccountsAPI = API.Accounts(API.TezosClient);

[@react.component]
let make = () => {
  let (_, setAccounts) = React.useContext(AccountsState.context);

  let (backupPhrase, setBackupPhrase) = React.useState(() => "");
  let (name, setName) = React.useState(() => "");
  let (password, setPassword) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setBackupPhrase(_ => text)}
      placeholder="24 words"
      value=backupPhrase
    />
    <TextInput
      onChangeText={text => setName(_ => text)}
      placeholder="alias"
      value=name
    />
    <TextInput
      onChangeText={text => setPassword(_ => text)}
      placeholder="password"
      secureTextEntry=true
      value=password
    />
    <Button
      onPress={_ =>
        AccountsAPI.addWithMnemonic(name, backupPhrase, ~password)
        ->Future.flatMapOk(_ => AccountsAPI.get())
        ->Future.get(result =>
            switch (result) {
            | Ok(value) => setAccounts(value)
            | Error(value) => Dialog.error(value)
            }
          )
      }
      title="Restore"
    />
  </View>;
};
