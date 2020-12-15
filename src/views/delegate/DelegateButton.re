open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~height=36.->dp,
          ~minWidth=103.->dp,
          ~paddingHorizontal=16.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~borderRadius=4.,
          (),
        ),
      "textButton": style(~color=Theme.colorLightHighEmphasis, ()),
      "textButtonDisabled": style(~color=Theme.colorLightDisabled, ()),
    })
  );

[@react.component]
let make =
    (~account as defaultAccount=?, ~disabled=false, ~style as styleFromProp=?) => {
  let theme = ThemeContext.useTheme();

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
      style=Style.(
        arrayOption([|
          Some(styles##button),
          Some(
            style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
          ),
          styleFromProp,
        |])
      )
      onPress
      disabled>
      <Typography.ButtonSecondary
        style=Style.(
          style(
            ~color=
              disabled
                ? theme.colors.primaryTextDisabled
                : theme.colors.primaryTextHighEmphasis,
            (),
          )
        )>
        (disabled ? I18n.btn#delegated : I18n.btn#delegate)->React.string
      </Typography.ButtonSecondary>
    </TouchableOpacity>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <DelegateView onPressCancel action={Delegate.Create(defaultAccount)} />
    </ModalAction>
  </>;
};
