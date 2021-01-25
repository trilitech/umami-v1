open ReactNative;

[@react.component]
let make = (~style=?) => {
  let setSeen = LogsContext.useSetSeen();
  let seen = LogsContext.useSeen();

  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => {
    openAction();
    true->setSeen;
  };

  <>
    <TouchableOpacity ?style onPress>
      <Typography.ButtonPrimary
        fontSize=12. colorStyle={!seen ? `error : `disabled}>
        I18n.btn#logs->React.string
      </Typography.ButtonPrimary>
    </TouchableOpacity>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <LogsView closeAction />
    </ModalAction>
  </>;
};
