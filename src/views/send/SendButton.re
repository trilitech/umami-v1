open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        StyleSheet.flatten([|
          style(~borderRadius=35., ~overflow=`hidden, ()),
          ShadowStyles.button,
        |]),
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
