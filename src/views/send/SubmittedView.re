open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~alignItems=`center, ()),
      "title": style(~marginBottom=6.->dp, ()),
      "body": style(~marginBottom=30.->dp, ~textAlign=`center, ()),
      "hashTitle": style(~marginBottom=2.->dp, ()),
      "addressContainer":
        style(
          ~flexDirection=`row,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
      "address": style(~marginRight=4.->dp, ()),
    })
  );

[@react.component]
let make = (~hash, ~onPressCancel, ~submitText=I18n.btn#ok) => {
  let addToast = LogsContext.useToast();

  <View style=styles##container>
    <Typography.Headline style=styles##title>
      I18n.title#operation_submited->React.string
    </Typography.Headline>
    <Typography.Body2 colorStyle=`mediumEmphasis style=styles##body>
      I18n.expl#operation->React.string
    </Typography.Body2>
    <Typography.Overline2 style=styles##hashTitle>
      I18n.t#operation_hash->React.string
    </Typography.Overline2>
    <View style=styles##addressContainer>
      <Typography.Address fontSize=16. style=styles##address>
        hash->React.string
      </Typography.Address>
      <ClipboardButton copied=I18n.log#address addToast data=hash />
    </View>
    <View style=FormStyles.formAction>
      <Buttons.FormPrimary text=submitText onPress=onPressCancel />
    </View>
  </View>;
};
