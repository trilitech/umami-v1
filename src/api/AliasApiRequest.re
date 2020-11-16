include ApiRequest;

/* ALIAS */

module AliasesAPI = API.Aliases(API.TezosClient);

/* Get list */

type getAliasesAPIRequest = t(array((string, string)));

let useGetAliases = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  React.useEffect1(
    () => {
      setRequest(_ => Loading);

      AliasesAPI.get(~config)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    [|setRequest|],
  );

  request;
};

/* Create */

type createAliasApiRequest = t(string);

let useCreateAlias = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  let sendRequest = (alias, address) => {
    setRequest(_ => Loading);

    AliasesAPI.add(~config, alias, address)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};

/* Delete */

type deleteAliasApiRequest = t(string);

let useDeleteAlias = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  let sendRequest = name => {
    setRequest(_ => Loading);

    AliasesAPI.delete(~config, name)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};
