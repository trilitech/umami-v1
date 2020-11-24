[@react.component]
let make = (~hoveredStyle=?, ~color=?, ~data) => {
  <IconButton
    ?hoveredStyle
    icon={(~color as colorin=?) => {
      let color = [color, colorin]->Common.Lib.Option.firstSome;
      Icons.Copy.build(~color?);
    }}
    onPress={_ => {Navigator.Clipboard.write(data)}}
  />;
};
