[@react.component]
let make = () => {
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();

  <>
    <BigButton
      title=I18n.btn#import_account
      icon=Icons.ArrowDown.build
      onPress
    />
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <ImportAccountOnboardingView closeAction />
    </ModalAction>
  </>;
};
