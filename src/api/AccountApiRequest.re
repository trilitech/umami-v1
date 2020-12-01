/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient);

/* Get */

let useLoad = requestState => {
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

  ApiRequest.useLoader(~get, ~kind=Logs.Account, ~requestState);
};

/* Set */

let useCreate =
  ApiRequest.useSetter(~set=AccountsAPI.create, ~kind=Logs.Account);

let useDelete =
  ApiRequest.useSetter(~set=AccountsAPI.delete, ~kind=Logs.Account);

type createInput = {
  name: string,
  mnemonics: string,
  password: string,
};

let useCreateWithMnemonics =
  ApiRequest.useSetter(
    ~set=
      (~config, {name, mnemonics, password}) =>
        AccountsAPI.addWithMnemonic(~config, name, mnemonics, ~password),
    ~kind=Logs.Account,
  );
