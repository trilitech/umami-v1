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
let make = (~zeroTez, ~action: Delegate.action, ~style as styleFromProp=?) => {
  let theme = ThemeContext.useTheme();

  let tooltipId =
    "delegate_button_"
    ++ Delegate.account(action)->Option.mapWithDefault("", a => a.address);

  let (textColor, backgroundColor, text, tooltip) =
    switch (action) {
    | _ when zeroTez => (
        theme.colors.primaryTextDisabled,
        theme.colors.primaryButtonBackground,
        I18n.btn#delegate,
        Some((tooltipId, I18n.expl#no_tez_no_delegation)),
      )

    | Create(_) => (
        theme.colors.primaryTextHighEmphasis,
        theme.colors.primaryButtonBackground,
        I18n.btn#delegate,
        None,
      )
    | Edit(_)
    | Delete(_) => (
        theme.colors.primaryTextDisabled,
        theme.colors.iconDisabled,
        I18n.btn#delegated,
        Some((tooltipId, I18n.btn#update_delegation)),
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
        ?tooltip
        disabled=zeroTez
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
