[@react.component]
let make = () => {
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();

  <>
    <BigButton
      title=I18n.btn#create_account_new
      icon=Icons.Add.build
      onPress
    />
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <CreateAccountOnboardingView closeAction />
    </ModalAction>
  </>;
};
