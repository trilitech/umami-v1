open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(~alignSelf=`flexStart, ~overflow=`hidden, ~borderRadius=4., ()),
      "pressable":
        style(
          ~height=34.->dp,
          ~minWidth=104.->dp,
          ~paddingHorizontal=16.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~action: Delegate.action, ~style as styleFromProp=?) => {
  let theme = ThemeContext.useTheme();

  let disabledLook =
    switch (action) {
    | Create(_) => false
    | Edit(_) => true
    | Delete(_) => true
    };

  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _e => openAction();

  <>
    <View
      style=Style.(
        arrayOption([|
          Some(styles##button),
          Some(
            style(
              ~backgroundColor=
                disabledLook
                  ? theme.colors.iconDisabled
                  : theme.colors.primaryButtonBackground,
              (),
            ),
          ),
          styleFromProp,
        |])
      )>
      <ThemedPressable
        style=Style.(arrayOption([|Some(styles##pressable)|]))
        isPrimary=true
        onPress
        accessibilityRole=`button>
        <Typography.ButtonPrimary
          style=Style.(
            style(
              ~color=
                disabledLook
                  ? theme.colors.primaryTextDisabled
                  : theme.colors.primaryTextHighEmphasis,
              (),
            )
          )>
          (disabledLook ? I18n.btn#delegated : I18n.btn#delegate)->React.string
        </Typography.ButtonPrimary>
      </ThemedPressable>
    </View>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <DelegateView closeAction action />
    </ModalAction>
  </>;
};
