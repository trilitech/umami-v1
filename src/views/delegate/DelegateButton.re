open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(~alignSelf=`flexStart, ~overflow=`hidden, ~borderRadius=4., ()),
      "pressable":
        style(
          ~height=36.->dp,
          ~minWidth=103.->dp,
          ~paddingHorizontal=16.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

[@react.component]
let make =
    (~account as defaultAccount=?, ~disabled=false, ~style as styleFromProp=?) => {
  let theme = ThemeContext.useTheme();

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
                disabled
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
        disabled>
        <Typography.ButtonPrimary
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
        </Typography.ButtonPrimary>
      </ThemedPressable>
    </View>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <DelegateView closeAction action={Delegate.Create(defaultAccount)} />
    </ModalAction>
  </>;
};
