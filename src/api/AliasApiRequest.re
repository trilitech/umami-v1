include ApiRequest;

/* ALIAS */

module AliasesAPI = API.Aliases(API.TezosClient);

/* Get list */

type getAliasesAPIRequest = t(array((string, string)));

let useGetAliases = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);

  React.useEffect1(
    () => {
      setRequest(_ => Loading);

      AliasesAPI.get()->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    [|setRequest|],
  );

  request;
};

/* Create */

type createAccountApiRequest = t(string);

let useCreateAlias = () => {
  let (request, setRequest) = React.useState(_ => NotAsked);

  let sendRequest = (alias, address) => {
    setRequest(_ => Loading);

    AliasesAPI.add(alias, address)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};
