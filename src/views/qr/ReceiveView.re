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
  let addToast = LogsContext.useToast();

  <ModalTemplate.Form
    headerRight={<ModalTemplate.HeaderButtons.Close onPress=onPressCancel />}>
    <Typography.Headline style=FormStyles.header>
      account.alias->React.string
    </Typography.Headline>
    <View style=styles##qrContainer>
      <View style=styles##qr>
        <QRCode value={account.address} size=200. />
      </View>
    </View>
    <View style=styles##addressContainer>
      <Typography.Address style=styles##address>
        account.address->React.string
      </Typography.Address>
      <ClipboardButton
        copied=I18n.log#address
        tooltipKey="QrView"
        addToast
        data={account.address}
      />
    </View>
  </ModalTemplate.Form>;
};
