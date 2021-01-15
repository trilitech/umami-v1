/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient);

/* Get */

let useLoad = requestState => {
  let get = (~settings, ()) =>
    AccountsAPI.get(~settings)
    ->Future.mapOk(response => {
        response
        ->Array.map(((alias, address)) => {
            let account: Account.t = {alias, address};
            (address, account);
          })
        ->Array.reverse
        ->Map.String.fromArray
      });

  ApiRequest.useLoader(~get, ~kind=Logs.Account, ~requestState);
};

/* Set */

let useCreate =
  ApiRequest.useSetter(~set=AccountsAPI.create, ~kind=Logs.Account);

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (~settings, {alias, address}: Account.t) =>
        AccountsAPI.add(~settings, alias, address),
    ~kind=Logs.Account,
  );

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
      (~settings, {name, mnemonics, password}) =>
        AccountsAPI.addWithMnemonic(~settings, name, mnemonics, ~password),
    ~kind=Logs.Account,
  );
