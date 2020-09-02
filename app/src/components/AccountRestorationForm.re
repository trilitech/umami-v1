open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

[@react.component]
let make = () => {
  let (_, setAccounts) = React.useContext(Accounts.context);

  let (backupPhrase, setBackupPhrase) = React.useState(() => "");
  let (name, setName) = React.useState(() => "");

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
    <Button
      onPress={
        _ =>
          API.Accounts.restore(backupPhrase, name)
          ->Future.flatMapOk(_ => API.Accounts.get())
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