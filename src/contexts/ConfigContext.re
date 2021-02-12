open UmamiCommon;
type config = {
  content: ConfigFile.t,
  write: (ConfigFile.t => ConfigFile.t) => unit,
  loaded: bool,
  sdkMain: Js.Promise.t(TezosSDK.t),
  sdkTest: Js.Promise.t(TezosSDK.t),
  network: AppSettings.network,
};

let initialState = {
  content: ConfigFile.dummy,
  write: _ => (),
  loaded: false,
  sdkMain: TezosSDK.init("", ""),
  sdkTest: TezosSDK.init("", ""),
  network: Testnet,
};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

[@react.component]
let make = (~children) => {
  let (content, setConfig) = React.useState(() => initialState.content);
  let (loaded, setLoaded) = React.useState(() => initialState.loaded);

  let (network, _) = React.useState(() => AppSettings.Testnet);

  let ((sdkMain, sdkTest), _) =
    React.useState(() => {
      let endpointMain =
        content.endpointMain
        ->Option.getWithDefault(ConfigFile.Default.endpointMain)
        ->ConfigFile.Default.mkSdkEndpoint;

      let endpointTest =
        content.endpointTest
        ->Option.getWithDefault(ConfigFile.Default.endpointTest)
        ->ConfigFile.Default.mkSdkEndpoint;

      let dir =
        ConfigFile.(
          content.sdkBaseDir->Option.getWithDefault(Default.sdkBaseDir)
        );
      (TezosSDK.init(dir, endpointMain), TezosSDK.init(dir, endpointTest));
    });

  let load = () => {
    System.Config.read()
    ->Future.get(conf => {
        switch (conf) {
        | Ok(conf) =>
          let conf = ConfigFile.parse(conf);
          setConfig(_ => conf);
          setLoaded(_ => true);
        | Error(e) =>
          Js.log(e);
          setLoaded(_ => true);
        }
      });
  };

  React.useEffect0(() => {
    load();
    None;
  });

  let write = f =>
    setConfig(c => {
      let c = f(c);
      c
      ->ConfigFile.toString
      ->Lib.Option.iter(c => c->System.Config.write->Future.get(_ => ()));
      c;
    });

  <Provider value={content, loaded, write, sdkMain, sdkTest, network}>
    children
  </Provider>;
};

let useContext = () => React.useContext(context);

let useWrite = () => {
  let store = useContext();
  store.write;
};

let useLoaded = () => {
  let store = useContext();
  store.loaded;
};

let useSettings = () => {
  let store = useContext();
  AppSettings.{
    config: store.content,
    sdk: {
      main: store.sdkMain,
      test: store.sdkTest,
    },
    network: store.network,
  };
};

let useResetConfig = () => {
  let {write} = useContext();
  () => {
    write(_ => ConfigFile.dummy);
  };
};

let useCleanSdkBaseDir = () => {
  let {content: {sdkBaseDir}} = useContext();
  () => {
    System.Client.reset(
      sdkBaseDir->Option.getWithDefault(ConfigFile.Default.sdkBaseDir),
    );
  };
};
