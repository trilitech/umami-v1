include ApiRequest;

/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient);
module ScannerAPI = API.Scanner(API.TezosClient, API.TezosExplorer);

/* Get list */

type getAccountsApiRequest = t(array((string, string)));

let useGetAccounts = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  React.useEffect1(
    () => {
      setRequest(_ => Loading);

      AccountsAPI.get(~config)
      ->Future.tapOk(Js.log)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    [|setRequest|],
  );

  request;
};

/* Create */

type createAccountApiRequest = t(string);

let useCreateAccount = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  let sendRequest = name => {
    setRequest(_ => Loading);

    AccountsAPI.create(~config, name)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};

/* Delete */

type deleteAccountApiRequest = t(string);

let useDeleteAccount = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  let sendRequest = name => {
    setRequest(_ => Loading);

    AccountsAPI.delete(~config, name)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};

/* CreateWithMnemonic */

type createAccountWithMnemonicApiRequest = t(string);

let useCreateAccountWithMnemonic = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  //let network = StoreContext.useNetwork();
  let config = ConfigContext.useConfig();

  let sendRequest = (name, mnemonic, ~password) => {
    setRequest(_ => Loading);

    AccountsAPI.addWithMnemonic(~config, name, mnemonic, ~password)
    ->Future.flatMapOk(_ =>
        (TezosClient.Network.Test, config)->ScannerAPI.scan(
          mnemonic,
          name ++ ".",
          ~derivationSchema="m/44'/1729'/?'/0'",
          ~index=0,
        )
      )
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};
