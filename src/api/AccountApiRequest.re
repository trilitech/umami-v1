/* ACCOUNT */

module AccountsAPI = API.Accounts(API.TezosExplorer);

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

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (~settings, renaming: TezosSDK.renameParams) =>
        API.Aliases.rename(~settings, renaming),
    ~kind=Logs.Account,
  );

let useDelete =
  ApiRequest.useSetter(~set=AccountsAPI.delete, ~kind=Logs.Account);
