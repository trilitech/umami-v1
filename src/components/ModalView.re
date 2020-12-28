open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "closeButton":
        style(~position=`absolute, ~right=20.->dp, ~top=20.->dp, ()),
      "modal":
        style(~width=642.->dp, ~alignSelf=`center, ~borderRadius=4., ()),
    })
  );

let formStyles =
  Style.(
    StyleSheet.create({
      "modal":
        style(
          ~paddingTop=45.->dp,
          ~paddingBottom=40.->dp,
          ~paddingHorizontal=110.->dp,
          (),
        ),
    })
  );

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

module ConfirmCloseModal = {
  [@react.component]
  let make = (~confirm, ~closeAction, ~visible) => {
    let {title, subtitle, cancelText, actionText, action} = confirm;

    let modal = React.useRef(Js.Nullable.null);

    let onPressCancel = _e => {
      modal.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(ModalAction.closeModal)
      ->ignore;
    };

    let theme = ThemeContext.useTheme();

    <ModalAction ref=modal visible onRequestClose=closeAction>
      <View
        style=Style.(
          array([|
            styles##modal,
            formStyles##modal,
            style(~backgroundColor=theme.colors.background, ()),
          |])
        )>
        <Typography.Headline style=FormStyles.header>
          title->React.string
        </Typography.Headline>
        {subtitle->ReactUtils.mapOpt(sub => {
           <Typography.Headline> sub->React.string </Typography.Headline>
         })}
        <View style=FormStyles.formAction>
          <Buttons.FormPrimary text=cancelText onPress=onPressCancel />
          <Buttons.FormPrimary
            onPress={_ => {
              closeAction();
              action();
            }}
            text=actionText
          />
        </View>
      </View>
    </ModalAction>;
  };
};

module CloseButton = {
  [@react.component]
  let make = (~closing) => {
    let theme = ThemeContext.useTheme();

    let (visibleModal, setVisibleModal) = React.useState(_ => false);
    let openAction = () => setVisibleModal(_ => true);
    let closeAction = () => setVisibleModal(_ => false);

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
      <TouchableOpacity onPress style=styles##closeButton>
        <Icons.Close size=36. color={theme.colors.iconMediumEmphasis} />
      </TouchableOpacity>
      confirm
    </>;
  };
};

module Base = {
  [@react.component]
  let make = (~children, ~closing=?, ~style as styleFromProp=?) => {
    let closeButton =
      closing->Belt.Option.map(closing => <CloseButton closing />);

    let theme = ThemeContext.useTheme();
    <View
      style=Style.(
        arrayOption([|
          styleFromProp,
          Some(styles##modal),
          Some(style(~backgroundColor=theme.colors.background, ())),
        |])
      )>
      closeButton->ReactUtils.opt
      children
    </View>;
  };
};

module Form = {
  [@react.component]
  let make = (~closing=?, ~children) => {
    <Base ?closing style=formStyles##modal> children </Base>;
  };
};

include Base;
