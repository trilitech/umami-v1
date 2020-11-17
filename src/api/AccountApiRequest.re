/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient);
module ScannerAPI = API.Scanner(API.TezosClient, API.TezosExplorer);

/* Get list */

let useLoad = ApiRequest.useLoader(AccountsAPI.get, Error.Account);

let useCreate = ApiRequest.useSetter(AccountsAPI.create, Error.Account);

let useGet = () => {
  let get = (~config, ()) => AccountsAPI.get(~config)->Future.tapOk(_ => ());

  ApiRequest.useGetter(get, Error.Account);
};

let useDelete =
  ApiRequest.useSetter(
    (~config, name) => AccountsAPI.delete(name, ~config),
    Error.Account,
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
    Error.Account,
  );
