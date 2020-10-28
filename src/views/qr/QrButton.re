[@react.component]
let make =
    (
      ~account,
      ~balanceRequest: option(BalanceApiRequest.balanceApiRequest)=?,
    ) => {
  let modal = React.useRef(Js.Nullable.null);

  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let onPress = _ => {
    openAction();
  };

  let onPressCancel = _e => {
    modal.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(ModalAction.closeModal)
    ->ignore;
  };

  <>
    <IconButton icon=`qr onPress />
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <ReceiveView account ?balanceRequest onPressCancel />
    </ModalAction>
  </>;
};
