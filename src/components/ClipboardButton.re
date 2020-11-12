[@react.component]
let make = (~data) => {
  <IconButton
    icon=Icons.Copy.build
    onPress={_ => {Navigator.Clipboard.write(data)}}
  />;
};
