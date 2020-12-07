open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module TokensAPI = API.Tokens(API.TezosClient, API.TezosExplorer);

[@react.component]
let make = () => {
  let config = ConfigContext.useConfig();
  let (kt1, setKt1) = React.useState(() => "");
  let (account, setAccount) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setKt1(_ => text)}
      placeholder="token KT1/alias"
      value=kt1
    />
    <TextInput
      onChangeText={text => setAccount(_ => text)}
      placeholder="account"
      value=account
    />
    <Button
      onPress={_ =>
        Tokens.makeGetBalance(
          account,
          "KT1BZ6cBooBYubKv4Z3kd7izefLXgwTrSfoG",
          kt1,
          (),
        )
        ->TokensAPI.callGetOperationOffline((Network.Test, config), _)
        ->Future.get(result =>
            switch (result) {
            | Ok(balance) => Dialog.error(balance)
            | Error(err) => Dialog.error(err)
            }
          )
      }
      title="Get balance"
    />
  </View>;
};
