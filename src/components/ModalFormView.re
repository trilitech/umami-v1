open ReactNative;

type confirm = {
  title: string,
  subtitle: option(string),
  cancelText: string,
  actionText: string,
  action: unit => unit,
};

type closing =
  | Close(unit => unit)
  | Confirm(confirm);

let confirm =
    (
      ~title=I18n.title#confirm_cancel,
      ~subtitle=?,
      ~cancelText=I18n.btn#go_back,
      ~actionText,
      action,
    ) =>
  Confirm({title, subtitle, cancelText, actionText, action});

module BackButton = {
  [@react.component]
  let make = (~back) => {
    let theme = ThemeContext.useTheme();
    back->ReactUtils.mapOpt(back => {
      <TouchableOpacity onPress={_ => back()}>
        <Icons.ArrowLeft size=36. color={theme.colors.iconMediumEmphasis} />
      </TouchableOpacity>
    });
  };
};

module ConfirmCloseModal = {
  [@react.component]
  let make = (~confirm, ~closeAction, ~visible) => {
    let {title, subtitle, cancelText, actionText, action} = confirm;
    <ModalDialogConfirm
      visible
      closeAction
      action={() => {
        closeAction();
        action();
      }}
      title
      ?subtitle
      cancelText
      actionText
    />;
  };
};

module CloseButton = {
  [@react.component]
  let make = (~closing) => {
    let theme = ThemeContext.useTheme();

    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let confirm =
      switch (closing) {
      | Close(_) => React.null
      | Confirm(confirm) =>
        <ConfirmCloseModal confirm visible=visibleModal closeAction />
      };

    let onPress = _ => {
      switch (closing) {
      | Close(f) => f()
      | Confirm(_) => openAction()
      };
    };

    <>
      <TouchableOpacity onPress>
        <Icons.Close size=36. color={theme.colors.iconMediumEmphasis} />
      </TouchableOpacity>
      confirm
    </>;
  };
};

[@react.component]
let make = (~closing=?, ~back=?, ~loading=?, ~children) => {
  let closeButton =
    closing->Belt.Option.map(closing => <CloseButton closing />);

  let backButton = back->Belt.Option.map(back => <BackButton back />);

  <ModalTemplate.Form headerRight=?closeButton headerLeft=?backButton ?loading>
    children
  </ModalTemplate.Form>;
};
