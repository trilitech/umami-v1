open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button": style(~alignItems=`center, ()),
      "iconContainer":
        style(
          ~width=70.->dp,
          ~height=70.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~borderRadius=35.,
          (),
        ),
      "textButton": style(~marginTop=6.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  let theme = ThemeContext.useTheme();

  let modal = React.useRef(Js.Nullable.null);
  let account = StoreContext.SelectedAccount.useGet();

  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let onPress = _e => openAction();

  let onPress = account->Belt.Option.map(_ => onPress);

  let onPressCancel = _e => {
    modal.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(ModalAction.closeModal)
    ->ignore;
  };

  <>
    <TouchableOpacity style=styles##button ?onPress>
      <View
        style=Style.(
          array([|
            styles##iconContainer,
            style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
          |])
        )>
        <Icons.Send
          size=24.
          color={
            account->Belt.Option.isSome
              ? theme.colors.primaryIconHighEmphasis
              : theme.colors.primaryIconDisabled
          }
        />
        <Typography.ButtonSecondary
          style=Style.(
            array([|
              styles##textButton,
              style(
                ~color=
                  account->Belt.Option.isSome
                    ? theme.colors.primaryTextHighEmphasis
                    : theme.colors.primaryTextDisabled,
                (),
              ),
            |])
          )>
          I18n.btn#send->React.string
        </Typography.ButtonSecondary>
      </View>
    </TouchableOpacity>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <SendView onPressCancel />
    </ModalAction>
  </>;
};
