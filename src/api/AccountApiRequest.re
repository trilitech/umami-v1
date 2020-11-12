include ApiRequest;

/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient);

/* Get list */

type getAccountsApiRequest = t(array((string, string)));

let useGetAccounts = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  React.useEffect1(
    () => {
      setRequest(_ => Loading);

      AccountsAPI.get(~config)
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

  let sendRequest = (name, mnemonic, ~password) => {
    setRequest(_ => Loading);

    AccountsAPI.addWithMnemonic(name, mnemonic, ~password)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};
