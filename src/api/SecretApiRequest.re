/* ACCOUNT */

module AccountsAPI = AccountApiRequest.AccountsAPI;

/* Get */

let useLoad = requestState => {
  let get = (~settings, ()) =>
    AccountsAPI.secrets(~settings)
    ->Option.mapWithDefault(Future.value(Result.Ok([||])), secrets => {
        Future.value(Result.Ok(secrets))
      })
    ->Future.mapOk(secrets =>
        secrets->Array.mapWithIndex(
          (index, {name, derivationScheme, addresses, legacyAddress}) =>
          Secret.{index, name, derivationScheme, addresses, legacyAddress}
        )
      );

  ApiRequest.useLoader(~get, ~kind=Logs.Account, ~requestState);
};

let useGetRecoveryPhrase = (~requestState as (request, setRequest), ~index) => {
  let get = (~settings, password) =>
    AccountsAPI.recoveryPhraseAt(~settings, index, ~password);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Account, ~setRequest, ());

  (request, getRequest);
};

/* Set */

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (
        ~settings,
        {index, name, derivationScheme, addresses, legacyAddress}: Secret.t,
      ) => {
        let secret =
          API.Secret.{name, derivationScheme, addresses, legacyAddress};
        AccountsAPI.updateSecretAt(secret, ~settings, index);
      },
    ~kind=Logs.Account,
  );
