/* ALIAS */

module AliasesAPI = API.Aliases(API.TezosClient);

/* Get list */

let useLoad = ApiRequest.useLoader(AliasesAPI.get, Error.Aliases);

/* Create */

let useCreate =
  ApiRequest.useSetter(
    (~config, (alias, address)) => AliasesAPI.add(~config, alias, address),
    Error.Aliases,
  );

/* Delete */

let useDelete = ApiRequest.useSetter(AliasesAPI.delete, Error.Aliases);
