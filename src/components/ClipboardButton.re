[@react.component]
let make = (~isPrimary=?, ~tooltipKey=?, ~copied, ~addToast, ~data, ~style=?) => {
  <IconButton
    ?isPrimary
    icon=Icons.Copy.build
    tooltip=?{
      tooltipKey->Option.map(k =>
        ("ClipboadButton" ++ k, I18n.tooltip#copy_clipboard)
      )
    }
    onPress={_ => {
      Navigator.Clipboard.write(data);
      addToast(Logs.info(I18n.log#copied_to_clipboard(copied)));
    }}
    ?style
  />;
};
