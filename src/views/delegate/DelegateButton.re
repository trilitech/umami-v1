open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~height=36.->dp,
          ~paddingHorizontal=16.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~borderRadius=4.,
          ~backgroundColor="#D8BC63",
          (),
        ),
      "textButton": style(~color="rgba(0,0,0,0.87)", ()),
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
      <Typography.ButtonSecondary style=styles##textButton>
        "DELEGATE"->React.string
      </Typography.ButtonSecondary>
    </TouchableOpacity>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <DelegateView onPressCancel />
    </ModalAction>
  </>;
};
