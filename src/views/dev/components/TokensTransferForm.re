open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module TokensAPI = API.Tokens(API.TezosClient, API.TezosExplorer);

[@react.component]
let make = () => {
  let config = ConfigContext.useConfig();
  let (token, setToken) = React.useState(() => "");
  let (src, setSrc) = React.useState(() => "");
  let (dst, setDst) = React.useState(() => "");
  let (amount, setAmount) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setToken(_ => text)}
      placeholder="token KT1/alias"
      value=token
    />
    <TextInput
      onChangeText={text => setSrc(_ => text)}
      placeholder="source"
      value=src
    />
    <TextInput
      onChangeText={text => setDst(_ => text)}
      placeholder="destination"
      value=dst
    />
    <TextInput
      onChangeText={text => setAmount(_ => text)}
      placeholder="amount"
      value=amount
    />
    <Button
      onPress={_ =>
        Tokens.makeTransfer(src, dst, int_of_string(amount), token, ())
        ->TokensAPI.simulate((Network.Test, config), _)
        ->Future.get(result =>
            switch (result) {
            | Ok(_) => Dialog.error("ok")
            | Error(err) => Dialog.error(err)
            }
          )
      }
      title="Transfer tokens"
    />
  </View>;
};
