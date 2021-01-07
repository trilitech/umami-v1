open ReactNative;

[@react.component]
let make =
    (
      ~title,
      ~titleDone,
      ~titleLoading,
      ~visible,
      ~closeAction,
      ~onPressConfirmDelete,
      ~request: ApiRequest.t('a),
    ) => {
  let onPressCancel = _e => closeAction();

  <>
    <ModalAction visible onRequestClose=closeAction>
      <ModalView.Form>
        {switch (request) {
         | Done(Ok(_result), _) =>
           <>
             <Typography.Headline style=FormStyles.header>
               titleDone->React.string
             </Typography.Headline>
             <View style=FormStyles.formAction>
               <Buttons.Form text=I18n.btn#ok onPress=onPressCancel />
             </View>
           </>
         | Done(Error(error), _) =>
           <>
             <Typography.Body1 colorStyle=`error>
               error->React.string
             </Typography.Body1>
             <View style=FormStyles.formAction>
               <Buttons.Form text=I18n.btn#ok onPress=onPressCancel />
             </View>
           </>
         | Loading(_) =>
           <ModalView.LoadingView title=titleLoading height=120 />
         | NotAsked =>
           <>
             <Typography.Headline style=FormStyles.header>
               title->React.string
             </Typography.Headline>
             <View style=FormStyles.formAction>
               <Buttons.Form text=I18n.btn#cancel onPress=onPressCancel />
               <Buttons.Form
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
