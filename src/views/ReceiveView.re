open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=20.->dp, ~textAlign=`center, ()),
      "separatorSmall": style(~height=2.->dp, ()),
      "separatorBig": style(~height=5.->dp, ()),
      "qrContainer":
        style(
          ~marginTop=30.->dp,
          ~marginBottom=8.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
      "qr": style(~backgroundColor="white", ~padding=10.->dp, ()),
      "closeAction":
        style(~position=`absolute, ~right=20.->dp, ~top=20.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~account: Account.t,
      ~balanceRequest: BalanceApiRequest.balanceApiRequest,
      ~onPressCancel,
    ) => {
  <ModalView>
    <>
      <Typography.Headline2 style=styles##title>
        "QR code"->React.string
      </Typography.Headline2>
      <AccountInfo account balanceRequest />
      <View style=styles##qrContainer>
        <View style=styles##qr>
          <QRCode value={account.address} size=200. />
        </View>
      </View>
      <View style=styles##closeAction>
        <TouchableOpacity onPress=onPressCancel>
          <Icon name=`close size=36. color=Theme.colorDarkMediumEmphasis />
        </TouchableOpacity>
      </View>
    </>
  </ModalView>;
};
