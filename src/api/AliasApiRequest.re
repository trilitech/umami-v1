/* ALIAS */

module AliasesAPI = API.Aliases(API.TezosClient);

/* Get list */

let useLoad = requestState => {
  let get = (~config, ()) =>
    AliasesAPI.get(~config)
    ->Future.mapOk(response => {
        response
        ->Belt.Array.map(((alias, address)) => {
            let account: Account.t = {alias, address};
            (address, account);
          })
        ->Belt.Map.String.fromArray
      });

  ApiRequest.useLoader(~get, ~kind=Logs.Aliases, ~requestState);
};

/* Create */

let useCreate =
  ApiRequest.useSetter(
    ~set=
      (~config, (alias, address)) =>
        AliasesAPI.add(~config, alias, address),
    ~kind=Logs.Aliases,
  );

/* Delete */

let useDelete =
  ApiRequest.useSetter(~set=AliasesAPI.delete, ~kind=Logs.Aliases);
