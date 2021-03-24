[@react.component]
let make =
    (~color=?, ~buttonText, ~modalTitle, ~onPressConfirmDelete, ~request) => {
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let icon = (~color as colorin=?) => {
    let color = [color, colorin]->UmamiCommon.Lib.Option.firstSome;
    Icons.Delete.build(~color?);
  };

  let onPress = _ => {
    openAction();
  };

  <>
    <Menu.Item text=buttonText icon onPress colorStyle=`error />
    <DeleteConfirmModal
      title=modalTitle
      visible=visibleModal
      onPressConfirmDelete
      closeAction
      request
    />
  </>;
};
