open ReactNative;
open SettingsComponents;

module OffBoardView = {
  module StateLenses = [%lenses
    type state = {
      confirm: bool,
      word: string,
    }
  ];
  module OffboardForm = ReForm.Make(StateLenses);

  let styles =
    Style.(
      StyleSheet.create({
        "modal":
          style(
            ~width=642.->dp,
            ~paddingTop=40.->dp,
            ~paddingBottom=44.->dp,
            ~paddingHorizontal=135.->dp,
            (),
          ),
        "title": style(~marginBottom=4.->dp, ~textAlign=`center, ()),
        "text": style(~marginBottom=24.->dp, ~textAlign=`center, ()),
        "touchable":
          style(
            ~marginBottom=24.->dp,
            ~alignSelf=`flexStart,
            ~flexDirection=`row,
            ~alignItems=`flexStart,
            (),
          ),
        "checkbox": style(~marginRight=14.->dp, ()),
        "input": style(~marginBottom=8.->dp, ()),
      })
    );

  [@react.component]
  let make = (~closeAction) => {
    let form: OffboardForm.api =
      OffboardForm.use(
        ~schema={
          OffboardForm.Validation.(
            Schema(
              true_(Confirm)
              + custom(
                  ({word}) => {
                    let fieldState: ReSchema.fieldState =
                      word == "wasabi"
                        ? Valid
                        : Error(
                            I18n.settings#danger_offboard_form_input_error,
                          );

                    fieldState;
                  },
                  Word,
                ),
            )
          );
        },
        ~onSubmit=
          ({state}) => {
            Js.log(state);

            None;
          },
        ~initialState={confirm: false, word: ""},
        (),
      );

    let onSubmit = _ => {
      form.submit();
    };

    let formFieldsAreValids =
      FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

    let closeButton =
      <ModalTemplate.HeaderButtons.Close onPress={_ => closeAction()} />;

    <ModalTemplate.Base style=styles##modal headerRight=closeButton>
      <Typography.Headline style=styles##title>
        I18n.settings#danger_offboard_form_title->React.string
      </Typography.Headline>
      <Typography.Body1
        colorStyle=`error fontWeightStyle=`black style=styles##text>
        I18n.settings#danger_offboard_form_text->React.string
      </Typography.Body1>
      <TouchableOpacity
        onPress={_ => form.handleChange(Confirm, !form.values.confirm)}
        style=styles##touchable
        activeOpacity=1.>
        <Checkbox value={form.values.confirm} style=styles##checkbox />
        <Typography.Overline2 fontWeightStyle=`regular>
          I18n.settings#danger_offboard_form_checkbox_label->React.string
        </Typography.Overline2>
      </TouchableOpacity>
      <View style=styles##input>
        <ThemedTextInput
          value={form.values.word}
          onValueChange={form.handleChange(Word)}
          placeholder=I18n.settings#danger_offboard_form_input_placeholder
        />
      </View>
      <Buttons.SubmitPrimary
        text=I18n.btn#confirm
        onPress=onSubmit
        style=FormStyles.formSubmit
        disabledLook={!formFieldsAreValids}
      />
    </ModalTemplate.Base>;
  };
};

module OffboardButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button": style(~width=104.->dp, ~height=34.->dp, ()),
      })
    );

  [@react.component]
  let make = () => {
    let theme = ThemeContext.useTheme();

    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _ => openAction();

    <>
      <Buttons.SubmitPrimary
        style=Style.(
          array([|
            styles##button,
            style(~backgroundColor=theme.colors.error, ()),
          |])
        )
        text=I18n.settings#danger_offboard_button
        onPress
      />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <OffBoardView closeAction />
      </ModalAction>
    </>;
  };
};

module StateLenses = [%lenses type state = {natviewerTest: string}];
module TokenBalanceForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "row": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
      "columnLeft":
        style(~flexGrow=7., ~flexShrink=7., ~flexBasis=0.->dp, ()),
      "columnRight": style(~marginLeft=32.->dp, ()),
      "section": style(~marginBottom=8.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  <Block title=I18n.settings#danger_title>
    <View style=styles##row>
      <ColumnLeft style=styles##columnLeft>
        <Typography.Body1 colorStyle=`error style=styles##section>
          I18n.settings#danger_offboard_section->React.string
        </Typography.Body1>
        <Typography.Body1 colorStyle=`error>
          I18n.settings#danger_offboard_text->React.string
        </Typography.Body1>
      </ColumnLeft>
      <ColumnRight style=styles##columnRight> <OffboardButton /> </ColumnRight>
    </View>
  </Block>;
};
