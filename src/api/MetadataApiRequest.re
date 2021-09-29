open Let;
include ApiRequest;

let useLoadMetadata = (~onErrorNotATokenContract, pkh) => {
  let (_, checkToken) = TokensApiRequest.useCheckTokenContract();

  let keepTaquitoErrors =
    fun
    | TokensAPI.NotFA12Contract(_)
    | MetadataAPI.NoTzip12Metadata(_) => false
    | _ => true;

  let buildContract = (config: ConfigContext.env) => {
    let%Await token = checkToken(pkh);
    let%Await () =
      if (token == `KFA1_2) {
        Promise.ok();
      } else {
        onErrorNotATokenContract();
        TokensAPI.NotFA12Contract((pkh :> string))->Promise.err;
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
