/* ALIAS */

module AliasesAPI = API.Aliases(API.TezosClient);

/* Get list */

let useLoad = ApiRequest.useLoader(AliasesAPI.get, Logs.Aliases);

/* Create */

let useCreate =
  ApiRequest.useSetter(
    (~config, (alias, address)) => AliasesAPI.add(~config, alias, address),
    Logs.Aliases,
  );

/* Delete */

let useDelete = ApiRequest.useSetter(AliasesAPI.delete, Logs.Aliases);

let useGet = () => {
  let setAliases = StoreContext.useSetAliases();
  let get = (~config, ()) =>
    AliasesAPI.get(~config)->Future.tapOk(res => setAliases(_ => res));
  ApiRequest.useGetter(get, Logs.Operation);
};
