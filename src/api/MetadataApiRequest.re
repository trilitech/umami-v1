open Let;
include ApiRequest;

let useLoadMetadata = pkh => {
  let keepTaquitoErrors =
    fun
    | MetadataAPI.NoTzip12Metadata(_) => false
    | _ => true;

  let buildContract = config => {
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
