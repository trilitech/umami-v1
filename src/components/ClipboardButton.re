[@react.component]
let make = (~s) => {
  <IconButton
    icon=Icons.Copy.build
    onPress={_ => {Navigator.Clipboard.write(s)}}
  />;
};
