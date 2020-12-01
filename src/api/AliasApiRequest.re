/* ALIAS */

module AliasesAPI = API.Aliases(API.TezosClient);

/* Get list */

let useLoad = ((request, setRequest)) => {
  let get = (~config) =>
    AliasesAPI.get(~config)
    ->Future.mapOk(response => {
        response
        ->Belt.Array.map(((alias, address)) => {
            let account: Account.t = {alias, address};
            (address, account);
          })
        ->Belt.Map.String.fromArray
      });

  ApiRequest.useStoreLoader(~get, ~kind=Logs.Aliases, ~request, ~setRequest);

  request;
};

/* Create */

let useCreate =
  ApiRequest.useStoreSetter(
    ~set=
      (~config, (alias, address)) =>
        AliasesAPI.add(~config, alias, address),
    ~kind=Logs.Aliases,
  );

/* Delete */

let useDelete =
  ApiRequest.useStoreSetter(~set=AliasesAPI.delete, ~kind=Logs.Aliases);
