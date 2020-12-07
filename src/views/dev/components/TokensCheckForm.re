open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module TokensAPI = API.Tokens(API.TezosClient, API.TezosExplorer);

[@react.component]
let make = () => {
  let config = ConfigContext.useConfig();
  let (kt1, setKt1) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setKt1(_ => text)}
      placeholder="token KT1/alias"
      value=kt1
    />
    <Button
      onPress={_ =>
        TokensAPI.checkTokenContract((Network.Test, config), kt1)
        ->Future.get(result =>
            result ? Dialog.error("OK") : Dialog.error("KO")
          )
      }
      title="Check token contract"
    />
  </View>;
};
