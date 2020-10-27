include ApiRequest;

module BalanceAPI = API.Balance(API.TezosClient);

type balanceApiRequest = t(string);

let useBalance = account => {
  let network = StoreContext.useNetwork();

  let (request, setRequest) = React.useState(_ => NotAsked);

  React.useEffect3(
    () => {
      setRequest(_ => Loading);

      network
      ->BalanceAPI.get(account)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    (network, account, setRequest),
  );

  request;
};
