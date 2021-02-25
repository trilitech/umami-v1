/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosClient, API.TezosExplorer);

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

let useLoadSecrets = requestState => {
  let get = (~settings, ()) =>
    AccountsAPI.secrets(~settings)
    ->Option.mapWithDefault(Future.value(Result.Ok([||])), secrets => {
        Future.value(Result.Ok(secrets))
      });

  ApiRequest.useLoader(~get, ~kind=Logs.Account, ~requestState);
};

/* Set */

let useCreate =
  ApiRequest.useSetter(~set=AccountsAPI.create, ~kind=Logs.Account);

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (~settings, renaming: TezosSDK.renameParams) =>
        TezosSDK.renameAliases(AppSettings.sdk(settings), renaming),
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
        AccountsAPI.restore(~settings, mnemonics, name, ~password, ()),
    ~kind=Logs.Account,
  );
