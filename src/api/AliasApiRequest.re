/* ALIAS */

module AliasesAPI = API.Aliases(API.TezosClient);

/* Get list */

let useLoad = () => {
  let (request, setRequest) = StoreContext.useAliasesRequestState();

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

let useCreate = () => {
  let resetAliases = StoreContext.useResetAliases();

  let set = (~config, (alias, address)) =>
    AliasesAPI.add(~config, alias, address)
    ->Future.tapOk(_ => resetAliases());

  ApiRequest.useSetter(set, Logs.Aliases, ());
};

/* Delete */

let useDelete = () => {
  let resetAliases = StoreContext.useResetAliases();

  let set = (~config, alias) =>
    AliasesAPI.delete(~config, alias)->Future.tapOk(_ => resetAliases());

  ApiRequest.useSetter(set, Logs.Aliases, ());
};
