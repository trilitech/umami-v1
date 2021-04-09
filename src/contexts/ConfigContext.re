open UmamiCommon;
type config = {
  content: ConfigFile.t,
  write: (ConfigFile.t => ConfigFile.t) => unit,
};

let initialState = {content: ConfigFile.dummy, write: _ => ()};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

let load = () => {
  switch (ConfigFile.read()->Js.Nullable.toOption) {
  | Some(conf) => ConfigFile.parse(conf)
  | None =>
    Js.log("No config to load. Using default config");
    ConfigFile.dummy;
  };
};

[@react.component]
let make = (~children) => {
  let (content, setConfig) = React.useState(() => load());

  let write = f =>
    setConfig(c => {
      let c = f(c);
      c->ConfigFile.toString->Lib.Option.iter(c => c->ConfigFile.write);
      c;
    });

  <Provider value={content, write}> children </Provider>;
};

let useContext = () => React.useContext(context);

let useWrite = () => {
  let store = useContext();
  store.write;
};

let useContent = () => {
  let store = useContext();
  store.content;
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
    System.Client.resetDir(
      sdkBaseDir->Option.getWithDefault(ConfigFile.Default.sdkBaseDir),
    )
    ->Future.tapOk(_ => LocalStorage.clear());
  };
};
