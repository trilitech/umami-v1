open ReactNative;
open SettingsComponents;

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
      "button": style(~width=104.->dp, ~height=34.->dp, ()),
    })
  );

[@react.component]
let make = () => {
  let onSubmit = _ => {
    Js.log("click");
  };

  let theme = ThemeContext.useTheme();

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
      <ColumnRight style=styles##columnRight>
        <Buttons.SubmitPrimary
          style=Style.(
            array([|
              styles##button,
              style(~backgroundColor=theme.colors.error, ()),
            |])
          )
          text=I18n.settings#danger_offboard_button
          onPress=onSubmit
        />
      </ColumnRight>
    </View>
  </Block>;
};
