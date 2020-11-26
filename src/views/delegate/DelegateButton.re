open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~height=36.->dp,
          ~paddingHorizontal=16.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~borderRadius=4.,
          ~backgroundColor="#D8BC63",
          (),
        ),
      "textButton": style(~color=Theme.colorLightHighEmphasis, ()),
      "textButtonDisabled": style(~color=Theme.colorLightDisabled, ()),
    })
  );

[@react.component]
let make =
    (~account as defaultAccount=?, ~disabled=false, ~style as styleFromProp=?) => {
  let modal = React.useRef(Js.Nullable.null);

  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let onPress = _e => {
    openAction();
  };

  let onPressCancel = _e => {
    modal.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(ModalAction.closeModal)
    ->ignore;
  };

  <>
    <TouchableOpacity
      style=Style.(arrayOption([|Some(styles##button), styleFromProp|]))
      onPress
      disabled>
      <Typography.ButtonSecondary
        style={disabled ? styles##textButtonDisabled : styles##textButton}>
        (disabled ? I18n.btn#delegated : I18n.btn#delegate)->React.string
      </Typography.ButtonSecondary>
    </TouchableOpacity>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <DelegateView
        onPressCancel
        action={DelegateView.Create(defaultAccount)}
      />
    </ModalAction>
  </>;
};
