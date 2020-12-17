open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(~borderRadius=35., ~overflow=`hidden, ())
        ->unsafeAddStyle({
            "boxShadow": "0 3px 5px -1px rgba(0, 0, 0, 0.2), 0 1px 18px 0 rgba(0, 0, 0, 0.12), 0 6px 10px 0 rgba(0, 0, 0, 0.14)",
          }),
      "iconContainer":
        style(
          ~width=70.->dp,
          ~height=70.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
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
    <View
      style=Style.(
        array([|
          styles##button,
          style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
        |])
      )>
      <ThemedPressable isPrimary=true style=styles##iconContainer ?onPress>
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
      </ThemedPressable>
    </View>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <SendView onPressCancel />
    </ModalAction>
  </>;
};
