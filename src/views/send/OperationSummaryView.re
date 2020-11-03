open ReactNative;

[@react.component]
let make = (~operation: TezosClient.Injection.operation, ~sender, ~recipient) => {
  switch (operation) {
  | Transaction(_t) =>
    <> <AccountInfo account=sender /> <AccountInfo account=recipient /> </>
  | Delegation(_d) => <View />
  };
};
