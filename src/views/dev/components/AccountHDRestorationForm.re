open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module AccountsAPI = API.Accounts(API.TezosClient, API.TezosExplorer);

[@react.component]
let make = () => {
  let (_, setAccounts) = React.useContext(AccountsState.context);

  let (backupPhrase, setBackupPhrase) = React.useState(() => "");
  let (derivationPath, setDerivationPath) =
    React.useState(() => "m/44'/1729'/0'/0'");
  let (name, setName) = React.useState(() => "");
  let settings = SdkContext.useSettings();

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
      onChangeText={text => setDerivationPath(_ => text)}
      placeholder="derivation path"
      value=derivationPath
    />
    <Button
      onPress={_ => {
        let future =
          if (derivationPath->Js.String2.includes("?")) {
            Js.log("scan");
            AccountsAPI.scan(
              ~settings,
              backupPhrase,
              name,
              ~derivationScheme=derivationPath,
              ~password="blerot",
              ~index=0,
              (),
            );
          } else {
            AccountsAPI.restore(
              ~settings,
              backupPhrase,
              name,
              ~derivationScheme=derivationPath,
              ~password="blerot",
              (),
            );
          };
        future
        ->Future.flatMapOk(_ => AccountsAPI.get(~settings))
        ->Future.get(result =>
            switch (result) {
            | Ok(value) => setAccounts(value)
            | Error(value) => Dialog.error(value)
            }
          );
      }}
      title="Restore"
    />
  </View>;
};
