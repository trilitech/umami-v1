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

  ApiRequest.useStoreLoader(~get, ~kind=Logs.Account, ~request, ~setRequest);

  request;
};

/* Set */

let useCreate =
  ApiRequest.useStoreSetter(~set=AccountsAPI.create, ~kind=Logs.Account);

let useDelete =
  ApiRequest.useStoreSetter(~set=AccountsAPI.delete, ~kind=Logs.Account);

type createInput = {
  name: string,
  mnemonics: string,
  password: string,
};

let useCreateWithMnemonics =
  ApiRequest.useStoreSetter(
    ~set=
      (~config, {name, mnemonics, password}) =>
        AccountsAPI.addWithMnemonic(~config, name, mnemonics, ~password),
    ~kind=Logs.Account,
  );
