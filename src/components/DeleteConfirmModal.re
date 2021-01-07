[@react.component]
let make =
    (
      ~title,
      ~visible,
      ~closeAction,
      ~onPressConfirmDelete,
      ~request: ApiRequest.t('a),
    ) => {
  let loading = request != ApiRequest.NotAsked;

  React.useEffect1(
    () => {
      request->ApiRequest.map(_ => closeAction());
      None;
    },
    [|request|],
  );

  <ModalDialogConfirm
    visible
    closeAction
    action=onPressConfirmDelete
    loading
    title
    cancelText=I18n.btn#cancel
    actionText=I18n.btn#delete
  />;
};
