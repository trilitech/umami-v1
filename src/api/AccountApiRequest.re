/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient);

/* Get */

let useLoad = ((request, setRequest)) => {
  let get = (~config) =>
    AccountsAPI.get(~config)
    ->Future.mapOk(response => {
        response
        ->Belt.Array.map(((alias, address)) => {
            let account: Account.t = {alias, address};
            (address, account);
          })
        ->Belt.Array.reverse
        ->Belt.Map.String.fromArray
      });

  ApiRequest.useStoreLoader(get, Logs.Account, request, setRequest);

  request;
};

/* Set */

let useCreate = (~sideEffect=?, ()) => {
  ApiRequest.useSetter(AccountsAPI.create, Logs.Account, ~sideEffect?, ());
};

let useDelete = (~sideEffect=?, ()) => {
  ApiRequest.useSetter(AccountsAPI.delete, Logs.Account, ~sideEffect?, ());
};

type createInput = {
  name: string,
  mnemonics: string,
  password: string,
};

let useCreateWithMnemonics = (~sideEffect=?, ()) => {
  ApiRequest.useSetter(
    (~config, {name, mnemonics, password}) =>
      AccountsAPI.addWithMnemonic(~config, name, mnemonics, ~password),
    Logs.Account,
    ~sideEffect?,
    (),
  );
};
