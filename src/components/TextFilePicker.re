module FileReader = {
  type t;
  [@bs.new] external createFileReader: unit => t = "FileReader";

  [@bs.send] external readAsText: (t, Js.t('a)) => unit = "readAsText";

  [@bs.set]
  external onload: (t, ReactEvent.Media.t => unit) => unit = "onload";
};

[@bs.send] external click: Dom.element => unit = "click";

[@react.component]
let make = (~text, ~primary=?, ~onChange as onChangeResult, ~accept=?) => {
  let inputRef = React.useRef(Js.Nullable.null);

  let onPress = _ => {
    inputRef.current->Js.Nullable.toOption->Option.map(click)->ignore;
  };

  let onChange = event => {
    let file = event->ReactEvent.Form.target##files->Array.get(0);
    switch (file) {
    | Some(file) =>
      let fileReader = FileReader.createFileReader();
      fileReader->FileReader.onload(event => {
        onChangeResult(event->ReactEvent.Media.target##result)
      });
      fileReader->FileReader.readAsText(file);
    | None => ()
    };
  };

  <>
    <ButtonAction onPress text ?primary icon=Icons.ArrowDown.build />
    <input
      ref={inputRef->ReactDOM.Ref.domRef}
      type_="file"
      multiple=false
      ?accept
      style={ReactDOM.Style.make(~display="none", ())}
      onChange
    />
  </>;
};
