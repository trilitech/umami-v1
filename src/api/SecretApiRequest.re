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

let useScanGlobal = (~requestState as (request, setRequest), ()) => {
  let get = (~settings, password) =>
    AccountsAPI.scanAll(~settings, ~password);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Account, ~setRequest, ());

  (request, getRequest);
};

/* Set */

type deriveInput = {
  name: string,
  index: int,
  password: string,
};

let useDerive =
  ApiRequest.useSetter(
    ~set=
      (~settings, {name, index, password}) =>
        AccountsAPI.derive(~settings, ~index, ~name, ~password),
    ~kind=Logs.Account,
  );

type createInput = {
  name: string,
  mnemonics: string,
  derivationScheme: string,
  password: string,
};

let useCreateWithMnemonics =
  ApiRequest.useSetter(
    ~set=
      (~settings, {name, mnemonics, derivationScheme, password}) =>
        AccountsAPI.restore(~settings, mnemonics, name, ~derivationScheme, ~password, ()),
    ~kind=Logs.Account,
  );

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

let useDelete =
  ApiRequest.useSetter(~set=AccountsAPI.deleteSecretAt, ~kind=Logs.Account);
