[@react.component]
let make = (~hoveredStyle=?, ~copied, ~addToast, ~color=?, ~data) => {
  <IconButton
    ?hoveredStyle
    icon={(~color as colorin=?) => {
      let color = [color, colorin]->Common.Lib.Option.firstSome;
      Icons.Copy.build(~color?);
    }}
    onPress={_ => {
      Navigator.Clipboard.write(data);
      addToast(Logs.info(I18n.log#copied_to_clipboard(copied)));
    }}
  />;
};
