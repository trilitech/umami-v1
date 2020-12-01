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

  ApiRequest.useStoreLoader(get, Logs.Aliases, request, setRequest);

  request;
};

/* Create */

let useCreate = (~sideEffect=?, ()) => {
  ApiRequest.useSetter(
    (~config, (alias, address)) => AliasesAPI.add(~config, alias, address),
    Logs.Aliases,
    ~sideEffect?,
    (),
  );
};

/* Delete */

let useDelete = (~sideEffect=?, ()) => {
  ApiRequest.useSetter(AliasesAPI.delete, Logs.Aliases, ~sideEffect?, ());
};
