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

  let account = StoreContext.SelectedAccount.useGet();

  let disabled = account->Belt.Option.isNone;

  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();
  let onPressCancel = _ => closeAction();

  <>
    <View
      style=Style.(
        array([|
          styles##button,
          style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
        |])
      )>
      <ThemedPressable
        isPrimary=true style=styles##iconContainer onPress disabled>
        <Icons.Send
          size=24.
          color={
            !disabled
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
                  !disabled
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
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <SendView onPressCancel />
    </ModalAction>
  </>;
};
