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

  let (textColor, backgroundColor, text) =
    switch (action) {
    | Create(_) => (
        theme.colors.primaryTextHighEmphasis,
        theme.colors.primaryButtonBackground,
        I18n.btn#delegate,
      )
    | Edit(_)
    | Delete(_) => (
        theme.colors.primaryTextDisabled,
        theme.colors.iconDisabled,
        I18n.btn#delegated,
      )
    };

  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _e => openAction();

  <>
    <View
      style=Style.(
        arrayOption([|
          Some(styles##button),
          Some(style(~backgroundColor, ())),
          styleFromProp,
        |])
      )>
      <ThemedPressable
        style=Style.(arrayOption([|Some(styles##pressable)|]))
        isPrimary=true
        onPress
        accessibilityRole=`button>
        <Typography.ButtonPrimary style=Style.(style(~color=textColor, ()))>
          text->React.string
        </Typography.ButtonPrimary>
      </ThemedPressable>
    </View>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <DelegateView closeAction action />
    </ModalAction>
  </>;
};
