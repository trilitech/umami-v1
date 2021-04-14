[@react.component]
let make = (~account, ~tooltipKey, ~style=?) => {
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();

  let onPressCancel = _ => closeAction();

  <>
    <IconButton
      tooltip=("QrButton" ++ tooltipKey, I18n.tooltip#show_qr)
      icon=Icons.Qr.build
      onPress
      ?style
    />
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <ReceiveView account onPressCancel />
    </ModalAction>
  </>;
};
