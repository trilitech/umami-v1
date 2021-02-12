open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module TokensAPI = API.Tokens(API.TezosClient, API.TezosExplorer);

[@react.component]
let make = () => {
  let settings = ConfigContext.useSettings();
  let (account, setAccount) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setAccount(_ => text)}
      placeholder="account"
      value=account
    />
    <Button
      onPress={_ =>
        Token.makeGetBalance(
          account,
          "KT1BUdnCMfBKdVxCKyBvMUqwLqm27EDGWskB",
          ~callback="KT1BZ6cBooBYubKv4Z3kd7izefLXgwTrSfoG",
          (),
        )
        ->TokensAPI.callGetOperationOffline(
            AppSettings.testOnly(settings),
            _,
          )
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
