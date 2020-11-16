type config = {
  content: ConfigFile.t,
  load: unit => unit,
  write: string => unit,
};

let initialState = {
  content: ConfigFile.default,
  load: _ => Js.log("non-initialized"),
  write: _ => (),
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

  let load = () => {
    System.Config.read()
    ->Future.get(conf => {
        switch (conf) {
        | Ok(conf) =>
          let conf = ConfigFile.parse(conf);
          setConfig(_ => conf);
        | Error(e) =>
          switch (ConfigFile.defaultToString()) {
          | Some(conf) =>
            Js.log(e);
            conf->System.Config.write->Future.get(_ => ());
          | None => ()
          }
        }
      });
  };

  let write = _ => {
    Js.log("Not implemeted yet");
  };

  <Provider value={content, load, write}> children </Provider>;
};

let useContext = () => React.useContext(context);

let useLoad = () => {
  let store = useContext();
  store.load;
};

let useConfig = () => {
  let store = useContext();
  store.content;
};
