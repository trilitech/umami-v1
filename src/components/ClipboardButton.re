[@react.component]
let make = (~hoveredStyle=?, ~copied, ~addLog, ~color=?, ~data) => {
  <IconButton
    ?hoveredStyle
    icon={(~color as colorin=?) => {
      let color = [color, colorin]->Common.Lib.Option.firstSome;
      Icons.Copy.build(~color?);
    }}
    onPress={_ => {
      Navigator.Clipboard.write(data);
      addLog(Logs.info(I18n.log#copied_to_clipboard(copied)));
    }}
  />;
};
