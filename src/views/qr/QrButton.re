[@react.component]
let make = (~account, ~style=?) => {
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();

  let onPressCancel = _ => closeAction();

  <>
    <IconButton icon=Icons.Qr.build onPress ?style />
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <ReceiveView account onPressCancel />
    </ModalAction>
  </>;
};
