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
          ~backgroundColor="#121212",
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
          ~backgroundColor="#FFF",
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  let modal = React.useRef(Js.Nullable.null);

  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);

  let onPress = _e => {
    openAction();
  };

  let onPressCancel = _e => {
    modal.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(ModalAction.closeModal)
    ->ignore;
  };

  <>
    <TouchableOpacity style=styles##button onPress>
      <View style=styles##iconContainer>
        <Icons.Add size=36. color=Colors.plainIconContent />
      </View>
      <Typography.Subtitle2>
        "CREATE NEW ACCOUNT"->React.string
      </Typography.Subtitle2>
    </TouchableOpacity>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <CreateAccountOnboardingView onPressCancel />
    </ModalAction>
  </>;
};
