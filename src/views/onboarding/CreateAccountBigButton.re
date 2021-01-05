open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~height=203.->dp,
          ~width=424.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~borderRadius=4.,
          (),
        ),
      "iconContainer":
        style(
          ~width=60.->dp,
          ~height=60.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~marginBottom=28.->dp,
          ~borderRadius=30.,
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();

  let cancel = _ => closeAction();

  let theme = ThemeContext.useTheme();

  <>
    <TouchableOpacity
      style=Style.(
        array([|
          styles##button,
          style(~backgroundColor=theme.colors.background, ()),
        |])
      )
      onPress>
      <View
        style=Style.(
          array([|
            styles##iconContainer,
            style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
          |])
        )>
        <Icons.Add size=36. color={theme.colors.primaryIconHighEmphasis} />
      </View>
      <Typography.Subtitle2>
        I18n.btn#create_account_new->React.string
      </Typography.Subtitle2>
    </TouchableOpacity>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <CreateAccountOnboardingView cancel />
    </ModalAction>
  </>;
};
