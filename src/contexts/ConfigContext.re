type config = {
  content: ConfigFile.t,
  write: string => unit,
  loaded: bool,
};

let initialState = {
  content: ConfigFile.default,
  write: _ => (),
  loaded: false,
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

  let load = () => {
    System.Config.read()
    ->Future.get(conf => {
        switch (conf) {
        | Ok(conf) =>
          let conf = ConfigFile.parse(conf);
          setConfig(_ => conf);
          setLoaded(_ => true);
        | Error(e) =>
          switch (ConfigFile.defaultToString()) {
          | Some(conf) =>
            Js.log(e);
            conf->System.Config.write->Future.get(_ => ());
            setLoaded(_ => true);
          | None => Js.Console.error("Unreadable default config")
          }
        }
      });
  };

  React.useEffect0(() => {
    load();
    None;
  });

  let write = _ => {
    Js.log("Not implemeted yet");
  };

  <Provider value={content, loaded, write}> children </Provider>;
};

let useContext = () => React.useContext(context);

let useLoaded = () => {
  let store = useContext();
  store.loaded;
};

let useConfig = () => {
  let store = useContext();
  store.content;
};
