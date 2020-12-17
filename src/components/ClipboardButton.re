[@react.component]
let make = (~isPrimary=?, ~copied, ~addToast, ~data) => {
  <IconButton
    ?isPrimary
    icon=Icons.Copy.build
    onPress={_ => {
      Navigator.Clipboard.write(data);
      addToast(Logs.info(I18n.log#copied_to_clipboard(copied)));
    }}
  />;
};
