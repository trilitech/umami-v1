open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=20.->dp, ~textAlign=`center, ()),
      "formAction":
        style(
          ~flexDirection=`row,
          ~justifyContent=`center,
          ~marginTop=24.->dp,
          (),
        ),
      "loadingView":
        style(
          ~height=120.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~title,
      ~titleDone,
      ~visible,
      ~closeAction,
      ~onPressConfirmDelete,
      ~request: ApiRequest.t('a),
    ) => {
  let modal = React.useRef(Js.Nullable.null);

  let onPressCancel = _e => {
    modal.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(ModalAction.closeModal)
    ->ignore;
  };

  let theme = ThemeContext.useTheme();

  <>
    <ModalAction ref=modal visible onRequestClose=closeAction>
      <ModalView.Form>
        {switch (request) {
         | Done(Ok(_result), _) =>
           <>
             <Typography.Headline2 style=styles##title>
               titleDone->React.string
             </Typography.Headline2>
             <View style=styles##formAction>
               <FormButton text=I18n.btn#ok onPress=onPressCancel />
             </View>
           </>
         | Done(Error(error), _) =>
           <>
             <Typography.Body1 colorStyle=`error>
               error->React.string
             </Typography.Body1>
             <View style=styles##formAction>
               <FormButton text=I18n.btn#ok onPress=onPressCancel />
             </View>
           </>
         | Loading(_) =>
           <View style=styles##loadingView>
             <ActivityIndicator
               animating=true
               size=ActivityIndicator_Size.large
               color={theme.colors.iconMediumEmphasis}
             />
           </View>
         | NotAsked =>
           <>
             <Typography.Headline2 style=styles##title>
               title->React.string
             </Typography.Headline2>
             <View style=styles##formAction>
               <FormButton text=I18n.btn#cancel onPress=onPressCancel />
               <FormButton text=I18n.btn#delete onPress=onPressConfirmDelete />
             </View>
           </>
         }}
      </ModalView.Form>
    </ModalAction>
  </>;
};
