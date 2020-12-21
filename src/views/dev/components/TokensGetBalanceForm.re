open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module TokensAPI = API.Tokens(API.TezosClient);

[@react.component]
let make = () => {
  let config = ConfigContext.useConfig();
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
          "KT1BZ6cBooBYubKv4Z3kd7izefLXgwTrSfoG",
          "KT1BUdnCMfBKdVxCKyBvMUqwLqm27EDGWskB",
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
