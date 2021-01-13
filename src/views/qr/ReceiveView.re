open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "qrContainer":
        style(
          ~marginTop=10.->dp,
          ~marginBottom=30.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
      "qr": style(~backgroundColor="white", ~padding=10.->dp, ()),
      "addressContainer":
        style(
          ~flexDirection=`row,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
      "address": style(~marginRight=4.->dp, ()),
      "closeAction":
        style(~position=`absolute, ~right=20.->dp, ~top=20.->dp, ()),
    })
  );

[@react.component]
let make = (~account: Account.t, ~onPressCancel) => {
  let theme = ThemeContext.useTheme();
  let addToast = LogsContext.useToast();

  <ModalTemplate.Form>
    <Typography.Headline style=FormStyles.header>
      account.alias->React.string
    </Typography.Headline>
    <View style=styles##qrContainer>
      <View style=styles##qr>
        <QRCode value={account.address} size=200. />
      </View>
    </View>
    <View style=styles##addressContainer>
      <Typography.Address fontSize=16. style=styles##address>
        account.address->React.string
      </Typography.Address>
      <ClipboardButton
        copied=I18n.log#address
        addToast
        data={account.address}
      />
    </View>
    <View style=styles##closeAction>
      <TouchableOpacity onPress=onPressCancel>
        <Icons.Close size=36. color={theme.colors.iconMediumEmphasis} />
      </TouchableOpacity>
    </View>
  </ModalTemplate.Form>;
};
