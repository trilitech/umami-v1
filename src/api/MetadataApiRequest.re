open Let;
include ApiRequest;

let useLoadMetadata = (~onErrorNotATokenContract, pkh) => {
  let (_, checkToken) = TokensApiRequest.useCheckTokenContract();

  let keepTaquitoErrors =
    fun
    | TokensApiRequest.NotFA12Contract(_)
    | MetadataAPI.NoTzip12Metadata(_) => false
    | _ => true;

  let buildContract = config => {
    let%FRes isToken = checkToken(pkh);
    let%FRes () =
      if (isToken) {
        FutureEx.ok();
      } else {
        onErrorNotATokenContract();
        TokensApiRequest.NotFA12Contract((pkh :> string))->FutureEx.err;
      };

    let toolkit = ReTaquito.Toolkit.create(config->ConfigUtils.endpoint);
    let%FRes contract = MetadataAPI.Tzip12.makeContract(toolkit, pkh);
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