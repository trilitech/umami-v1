[@react.component]
let make = () => {
  let _errors = ErrorsContext.useErrors();

  let modal = React.useRef(Js.Nullable.null);
  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let onPress = _ => {
    openAction();
  };

  <>
    <FormButton text="ERRORS" onPress />
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <ErrorsView />
    </ModalAction>
  </>;
};
