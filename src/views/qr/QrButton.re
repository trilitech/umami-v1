[@react.component]
let make = (~account, ~showBalance=true) => {
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();

  let onPressCancel = _ => closeAction();

  <>
    <IconButton icon=Icons.Qr.build onPress />
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <ReceiveView account onPressCancel showBalance />
    </ModalAction>
  </>;
};
