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
      "textButton": style(~marginTop=6.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  let modal = React.useRef(Js.Nullable.null);
  let account = StoreContext.useAccount();

  let (visibleModal, setVisibleModal) = React.useState(_ => false);
  let openAction = () => setVisibleModal(_ => true);
  let closeAction = () => setVisibleModal(_ => false);
  let addLog = ErrorsContext.useAdd();

  let onPress = _e => {
    /* addLog({ */
    /*   origin: Logs.Operation, */
    /*   kind: Logs.Error, */
    /*   timestamp: Js.Date.now(), */
    /*   msg: "Bad error about something", */
    /* }); */

    addLog({
      origin: Logs.Operation,
      kind: Logs.Info,
      timestamp: Js.Date.now(),
      msg: "Nice message about something",
    });

    openAction();
  };
  let onPress = account->Belt.Option.map(_ => onPress);

  let (iconColor, textColor) = {
    switch (account) {
    | Some(_) => (Colors.plainIconContent, Theme.colorLightHighEmphasis)
    | None => (Theme.colorLightDisabled, Theme.colorLightDisabled)
    };
  };

  let onPressCancel = _e => {
    modal.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(ModalAction.closeModal)
    ->ignore;
  };

  <>
    <TouchableOpacity style=styles##button ?onPress>
      <View style=styles##iconContainer>
        <Icons.Send size=24. color=iconColor />
        <Typography.ButtonSecondary
          style=Style.(
            array([|styles##textButton, style(~color=textColor, ())|])
          )>
          I18n.btn#send->React.string
        </Typography.ButtonSecondary>
      </View>
    </TouchableOpacity>
    <ModalAction ref=modal visible=visibleModal onRequestClose=closeAction>
      <SendView onPressCancel />
    </ModalAction>
  </>;
};
