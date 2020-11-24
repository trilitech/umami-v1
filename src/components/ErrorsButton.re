open ReactNative;

[@react.component]
let make = (~style=?) => {
  let _errors = ErrorsContext.useLogs();
  let setSeen = ErrorsContext.useSetSeen();
  let seen = ErrorsContext.useSeen();

  let modal = React.useRef(Js.Nullable.null);
  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let onPress = _ => {
    openAction();
    true->setSeen;
  };

  <>
    <TouchableOpacity ?style onPress>
      <Typography.ButtonPrimary
        fontSize=12. colorStyle={!seen ? `error : `disabled}>
        I18n.btn#error_logs->React.string
      </Typography.ButtonPrimary>
    </TouchableOpacity>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <ErrorsView />
    </ModalAction>
  </>;
};
