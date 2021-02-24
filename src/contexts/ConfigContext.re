open UmamiCommon;
type config = {
  content: ConfigFile.t,
  write: (ConfigFile.t => ConfigFile.t) => unit,
  loaded: bool,
  sdkMain: TezosSDK.t,
  sdkTest: TezosSDK.t,
  network: AppSettings.network,
};

let initialState = {
  content: ConfigFile.dummy,
  write: _ => (),
  loaded: false,
  sdkMain: TezosSDK.dummySdk,
  sdkTest: TezosSDK.dummySdk,
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

  let (sdkMain, setSdkMain) = React.useState(() => TezosSDK.dummySdk);
  let (sdkTest, setSdkTest) = React.useState(() => TezosSDK.dummySdk);

  let loadSdk = (conf: ConfigFile.t) => {
    let endpointMain =
      conf.endpointMain
      ->Option.getWithDefault(ConfigFile.Default.endpointMain);

    let endpointTest =
      conf.endpointTest
      ->Option.getWithDefault(ConfigFile.Default.endpointTest);

    let dir =
      ConfigFile.(conf.sdkBaseDir->Option.getWithDefault(Default.sdkBaseDir));

    let pMain =
      TezosSDK.init(dir, endpointMain)
      |> Js.Promise.then_(sdk => {
           setSdkMain(_ => sdk);
           Js.Promise.resolve();
         });

    let pTest =
      TezosSDK.init(dir, endpointTest)
      |> Js.Promise.then_(sdk => {
           setSdkTest(_ => sdk);
           Js.Promise.resolve();
         });

    Js.Promise.all([|pMain, pTest|])
    |> Js.Promise.then_(_ => setLoaded(_ => true)->Js.Promise.resolve)
    |> ignore;
  };

  let load = () => {
    System.Config.read()
    ->Future.map(conf => {
        switch (conf) {
        | Ok(conf) =>
          let conf = ConfigFile.parse(conf);
          setConfig(_ => conf);
          conf;
        | Error(e) =>
          Js.log(e);
          setLoaded(_ => true);
          ConfigFile.dummy;
        }
      })
    ->Future.get(conf => loadSdk(conf));
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
