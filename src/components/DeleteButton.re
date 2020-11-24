[@react.component]
let make =
    (
      ~color=?,
      ~buttonText,
      ~modalTitle,
      ~modalTitleDone,
      ~onPressConfirmDelete,
      ~request,
    ) => {
  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let icon = (~color as colorin=?) => {
    let color = [color, colorin]->Common.Lib.Option.firstSome;
    Icons.Delete.build(~color?);
  };

  let onPress = _ => {
    openAction();
  };

  <>
    <Menu.Item text=buttonText icon onPress />
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
