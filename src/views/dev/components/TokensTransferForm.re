open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module TokensAPI = API.Tokens(API.TezosExplorer);

[@react.component]
let make = () => {
  let settings = SdkContext.useSettings();
  let (source, setSrc) = React.useState(() => "");
  let (destination, setDst) = React.useState(() => "");
  let (amount, setAmount) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setSrc(_ => text)}
      placeholder="source"
      value=source
    />
    <TextInput
      onChangeText={text => setDst(_ => text)}
      placeholder="destination"
      value=destination
    />
    <TextInput
      onChangeText={text => setAmount(_ => text)}
      placeholder="amount"
      value=amount
    />
    <Button
      onPress={_ =>
        Token.(
          makeSingleTransfer(
            ~source,
            ~destination,
            ~amount=int_of_string(amount),
            ~contract="KT1BUdnCMfBKdVxCKyBvMUqwLqm27EDGWskB",
            (),
          )
          ->transfer
        )
        ->TokensAPI.simulate(AppSettings.testOnly(settings), _)
        ->Future.get(result =>
            switch (result) {
            | Ok(_) => Dialog.error("ok")
            | Error(err) => Dialog.error(err)
            }
          )
      }
      title="Simulate transfer"
    />
  </View>;
};
