open ReactNative;

[@react.component]
let make =
    (
      ~visible,
      ~closeAction,
      ~action,
      ~loading=?,
      ~title,
      ~subtitle=?,
      ~cancelText,
      ~actionText,
    ) => {
  <ModalAction visible onRequestClose=closeAction>
    <ModalTemplate.Dialog>
      <Typography.Headline style=FormStyles.header>
        title->React.string
      </Typography.Headline>
      {subtitle->ReactUtils.mapOpt(sub => {
         <Typography.Headline> sub->React.string </Typography.Headline>
       })}
      <View style=FormStyles.formAction>
        <Buttons.Form
          text=cancelText
          onPress={_ => closeAction()}
          disabled=?loading
        />
        <Buttons.Form onPress={_ => action()} text=actionText ?loading />
      </View>
    </ModalTemplate.Dialog>
  </ModalAction>;
};
