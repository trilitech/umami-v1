open ReactNative;

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

[@react.component]
let make = () => {
  let accounts = StoreContext.useAccounts();

  <>
    <Page>
      {accounts->Belt.Option.mapWithDefault(<LoadingView />, accounts => {
         accounts
         ->Belt.Map.String.valuesToArray
         ->Belt.Array.map(account =>
             <AddressBookRowItem key={account.address} account />
           )
         ->React.array
       })}
    </Page>
  </>;
};
