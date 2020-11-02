open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button": style(~alignItems=`center, ()),
      "iconContainer":
        style(
          ~width=70.->dp,
          ~height=70.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~borderRadius=35.,
          ~backgroundColor="#D8BC63",
          (),
        ),
      "textButton": style(~marginTop=6.->dp, ~color="rgba(0,0,0,0.87)", ()),
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
        <Icons.Send size=24. color=Colors.plainIconContent />
        <Typography.ButtonSecondary style=styles##textButton>
          "SEND"->React.string
        </Typography.ButtonSecondary>
      </View>
    </TouchableOpacity>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <SendView onPressCancel />
    </ModalAction>
  </>;
};
