[@react.component]
let make = (~title, ~titleDone, ~onPressConfirmDelete, ~request) => {
  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let onPress = _ => {
    openAction();
  };

  <>
    <IconButton icon=`delete onPress />
    <DeleteConfirmModal
      title
      titleDone
      visible=visibleModal
      onPressConfirmDelete
      closeAction
      request
    />
  </>;
};
