include ApiRequest;

/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient);

/* Get list */

type getAccountsApiRequest = t(array((string, string)));

let useGetAccounts = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);

  React.useEffect1(
    () => {
      setRequest(_ => Loading);

      AccountsAPI.get()->Future.get(result => setRequest(_ => Done(result)));

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

  let sendRequest = name => {
    setRequest(_ => Loading);

    AccountsAPI.create(name)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};
