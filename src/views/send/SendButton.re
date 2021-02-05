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

  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();

  <>
    <View
      style=Style.(
        array([|
          styles##button,
          style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
        |])
      )>
      <ThemedPressable
        isPrimary=true
        style=styles##iconContainer
        onPress
        accessibilityRole=`button>
        <Icons.Send size=24. color={theme.colors.primaryIconHighEmphasis} />
        <Typography.ButtonSecondary
          fontSize=13.
          style=Style.(
            array([|
              styles##textButton,
              style(~color=theme.colors.primaryTextHighEmphasis, ()),
            |])
          )>
          I18n.btn#send->React.string
        </Typography.ButtonSecondary>
      </ThemedPressable>
    </View>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <SendView closeAction />
    </ModalAction>
  </>;
};
