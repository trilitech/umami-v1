open ReactNative;

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

  <>
    <ModalAction ref=modal visible onRequestClose=closeAction>
      <ModalView.Form>
        {switch (request) {
         | Done(Ok(_result), _) =>
           <>
             <Typography.Headline style=FormStyles.header>
               titleDone->React.string
             </Typography.Headline>
             <View style=FormStyles.formAction>
               <Buttons.FormPrimary text=I18n.btn#ok onPress=onPressCancel />
             </View>
           </>
         | Done(Error(error), _) =>
           <>
             <Typography.Body1 colorStyle=`error>
               error->React.string
             </Typography.Body1>
             <View style=FormStyles.formAction>
               <Buttons.FormPrimary text=I18n.btn#ok onPress=onPressCancel />
             </View>
           </>
         | Loading(_) =>
           <ModalView.LoadingView title=I18n.title#simulation height=120 />
         | NotAsked =>
           <>
             <Typography.Headline style=FormStyles.header>
               title->React.string
             </Typography.Headline>
             <View style=FormStyles.formAction>
               <Buttons.FormPrimary
                 text=I18n.btn#cancel
                 onPress=onPressCancel
               />
               <Buttons.FormPrimary
                 text=I18n.btn#delete
                 onPress=onPressConfirmDelete
               />
             </View>
           </>
         }}
      </ModalView.Form>
    </ModalAction>
  </>;
};
