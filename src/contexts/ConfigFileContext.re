let fromFile = ConfigContext.fromFile;

type configFileState = {
  configFile: ConfigFile.t,
  getConfigWithDefaults: unit => ConfigContext.env,
  write: (ConfigFile.t => ConfigFile.t) => unit,
};

let initialState = {
  configFile: ConfigFile.dummy,
  write: _ => (),
  getConfigWithDefaults: _ => ConfigFile.dummy->fromFile,
};

let context = React.createContext(initialState);

module Provider = {
  // Why this boilerplate code
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

[@react.component]
let make = (~children) => {
  let (configFile, setConfig) = React.useState(ConfigContext.load);

  let write = f =>
    setConfig(c => {
      let c = f(c);
      c->ConfigFile.Storage.set;
      c;
    });
  ();

  let getConfigWithDefaults = () => configFile->fromFile;

  <Provider value={configFile, write, getConfigWithDefaults}>
    children
  </Provider>;
};

let useConfigFile = () => React.useContext(context);
