open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "closeButton":
        style(~position=`absolute, ~right=20.->dp, ~top=20.->dp, ()),
      "modal":
        style(~width=642.->dp, ~alignSelf=`center, ~borderRadius=4., ()),
      "modalForm":
        style(
          ~width=642.->dp,
          ~paddingTop=45.->dp,
          ~paddingBottom=40.->dp,
          ~paddingHorizontal=110.->dp,
          (),
        ),
      "modalDialog":
        style(
          ~width=522.->dp,
          ~paddingTop=40.->dp,
          ~paddingBottom=40.->dp,
          ~paddingHorizontal=50.->dp,
          (),
        ),

      "loadingView":
        style(
          ~height=400.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
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

module LoadingView = {
  [@react.component]
  let make = (~title, ~height=?) => {
    let theme = ThemeContext.useTheme();
    <View
      style={Style.arrayOption([|
        Some(styles##loadingView),
        height->Belt.Option.map(height =>
          Style.style(~height=height->string_of_int, ())
        ),
      |])}>
      <Typography.Headline style=FormStyles.header>
        title->React.string
      </Typography.Headline>
      <ActivityIndicator
        animating=true
        size=ActivityIndicator_Size.large
        color={theme.colors.iconMediumEmphasis}
      />
    </View>;
  };
};

module BackButton = {
  [@react.component]
  let make = (~back) => {
    let theme = ThemeContext.useTheme();
    back->ReactUtils.mapOpt(back => {
      <TouchableOpacity onPress={_ => back()} style=FormStyles.topLeftButton>
        <Icons.ArrowLeft size=36. color={theme.colors.iconMediumEmphasis} />
      </TouchableOpacity>
    });
  };
};

module ConfirmCloseModal = {
  [@react.component]
  let make = (~confirm, ~closeAction, ~visible) => {
    let {title, subtitle, cancelText, actionText, action} = confirm;

    let onPressCancel = _e => closeAction();

    let theme = ThemeContext.useTheme();

    <ModalAction visible onRequestClose=closeAction>
      <View
        style=Style.(
          array([|
            styles##modal,
            styles##modalDialog,
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
          <Buttons.Form text=cancelText onPress=onPressCancel />
          <Buttons.Form
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
          Some(styles##modal),
          Some(style(~backgroundColor=theme.colors.background, ())),
          styleFromProp,
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
    <Base ?closing style=styles##modalForm> children </Base>;
  };
};

include Base;
