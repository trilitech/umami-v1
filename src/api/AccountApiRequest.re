/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient);
module ScannerAPI = API.Scanner(API.TezosClient, API.TezosExplorer);

/* Get list */

let useLoad = ApiRequest.useLoader(AccountsAPI.get, Logs.Account);

let useCreate = ApiRequest.useSetter(AccountsAPI.create, Logs.Account);

let useGet = () => {
  let get = (~config, ()) =>
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

  ApiRequest.useGetter(get, Logs.Account);
};

let useDelete =
  ApiRequest.useSetter(
    (~config, name) => AccountsAPI.delete(name, ~config),
    Logs.Account,
  );

type createInput = {
  name: string,
  mnemonics: string,
  password: string,
};

let useCreateWithMnemonics =
  ApiRequest.useSetter(
    (~config, {name, mnemonics, password}) =>
      AccountsAPI.addWithMnemonic(~config, name, mnemonics, ~password),
    Logs.Account,
  );
