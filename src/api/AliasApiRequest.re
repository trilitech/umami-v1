/* ALIAS */

module AliasesAPI = API.Aliases(API.TezosClient);

/* Get list */

let useLoad = requestState => {
  let get = (~settings, ()) =>
    AliasesAPI.get(~settings)
    ->Future.mapOk(response => {
        response
        ->Array.map(((alias, address)) => {
            let account: Account.t = {alias, address};
            (address, account);
          })
        ->Array.reverse
        ->Map.String.fromArray
      });

  ApiRequest.useLoader(~get, ~kind=Logs.Aliases, ~requestState);
};

/* Create */

let useCreate =
  ApiRequest.useSetter(
    ~set=
      (~settings, (alias, address)) =>
        AliasesAPI.add(~settings, alias, address),
    ~kind=Logs.Aliases,
  );

/* Update */

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (~settings, renaming: TezosSDK.renameParams) =>
        TezosSDK.renameAliases(AppSettings.sdk(settings), renaming),
    ~kind=Logs.Aliases,
  );

/* Delete */

let useDelete =
  ApiRequest.useSetter(~set=AliasesAPI.delete, ~kind=Logs.Aliases);
