let initMigration = () => {
  let version =
    switch (LocalStorage.Version.get()) {
    | Ok(v) => v
    | Error(_) =>
      Js.log("Storage version not found, using 1.0 as base");
      Version.mk(1, 0);
    };

  switch (Migration.init(version)) {
  | Ok () => LocalStorage.Version.set(Migration.currentVersion)
  | Error(_) => ()
  };
};

let load = () => {
  initMigration();

  switch (ConfigFile.read()) {
  | Ok(conf) => conf
  | Error(_) =>
    Js.log("No config to load. Using default config");
    ConfigFile.dummy;
  };
};

type configFileState = {
  configFile: ConfigFile.t,
  write: (ConfigFile.t => ConfigFile.t) => unit,
};

let initialState = {configFile: ConfigFile.dummy, write: _ => ()};

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
  let (configFile, setConfig) = React.useState(load);

  let write = f =>
    setConfig(c => {
      let c = f(c);
      c->ConfigFile.Storage.set;
      c;
    });
  ();

  <Provider value={configFile, write}> children </Provider>;
};

let useConfigFile = () => React.useContext(context);
