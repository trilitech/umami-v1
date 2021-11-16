open Let;
include ApiRequest;

let useLoadMetadata = (~onErrorNotATokenContract, pkh) => {
  let (_, checkToken) = TokensApiRequest.useCheckTokenContract();

  let keepTaquitoErrors =
    fun
    | TokensAPI.NotFAContract(_)
    | MetadataAPI.NoTzip12Metadata(_) => false
    | _ => true;

  let buildContract = (config: ConfigContext.env) => {
    let%Await token = checkToken(pkh);
    let%Await () =
      switch (token) {
      | #TokenContract.kind => Promise.ok()
      | _ =>
        onErrorNotATokenContract();
        TokensAPI.NotFAContract((pkh :> string))->Promise.err;
      };

    let toolkit = ReTaquito.Toolkit.create(config.network.endpoint);
    let%Await contract = MetadataAPI.Tzip12.makeContract(toolkit, pkh);
    MetadataAPI.Tzip12.read(contract, 0);
  };

  let get = (~config, ()) => {
    buildContract(config);
  };

  let requestState = React.useState(() => ApiRequest.NotAsked);

  ApiRequest.useLoader(
    ~get,
    ~kind=Logs.Tokens,
    ~requestState,
    ~keepError=keepTaquitoErrors,
    (),
  );
};
