type config = {
  config: ConfigFile.t,
  sdkMain: TezosSDK.t,
  sdkTest: TezosSDK.t,
  network: AppSettings.network,
};

let initialState = {
  config: ConfigFile.dummy,
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
let make = (~empty, ~children) => {
  let config = ConfigContext.useContent();

  let (network, _) = React.useState(() => AppSettings.Testnet);

  let (sdkMain, setSdkMain) = React.useState(() => TezosSDK.dummySdk);
  let (sdkTest, setSdkTest) = React.useState(() => TezosSDK.dummySdk);
  let (loaded, setLoaded) = React.useState(() => false);

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
      TezosSDK.init(System.Path.toString(dir), endpointMain)
      |> Js.Promise.then_(sdk => {
           setSdkMain(_ => sdk);
           Js.Promise.resolve();
         });

    let pTest =
      TezosSDK.init(System.Path.toString(dir), endpointTest)
      |> Js.Promise.then_(sdk => {
           setSdkTest(_ => sdk);
           Js.Promise.resolve();
         });

    Js.Promise.all([|pMain, pTest|])
    |> Js.Promise.then_(_ => setLoaded(_ => true)->Js.Promise.resolve)
    |> ignore;
  };

  let load = () => {
    loadSdk(config);
  };

  React.useEffect0(() => {
    load();
    None;
  });

  <Provider value={config, sdkMain, sdkTest, network}>
    {loaded ? children : empty()}
  </Provider>;
};

let useContext = () => React.useContext(context);

let useSettings = () => {
  let store = useContext();
  AppSettings.{
    config: store.config,
    sdk: {
      main: store.sdkMain,

      test: store.sdkTest,
    },
    network: store.network,
  };
};
