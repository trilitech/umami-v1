[@react.component]
let make =
    (
      ~buttonText,
      ~modalTitle,
      ~modalTitleDone,
      ~onPressConfirmDelete,
      ~request,
    ) => {
  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let onPress = _ => {
    openAction();
  };

  <>
    <Menu.Item text=buttonText icon=Icons.Delete.build onPress />
    <DeleteConfirmModal
      title=modalTitle
      titleDone=modalTitleDone
      visible=visibleModal
      onPressConfirmDelete
      closeAction
      request
    />
  </>;
};
