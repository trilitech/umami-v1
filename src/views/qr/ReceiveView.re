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
let make = (~account: Account.t, ~showBalance, ~onPressCancel) => {
  <ModalView.Form>
    <>
      <Typography.Headline2 style=styles##title>
        account.alias->React.string
      </Typography.Headline2>
      <AccountInfo account showAlias=false showBalance />
      <View style=styles##qrContainer>
        <View style=styles##qr>
          <QRCode value={account.address} size=200. />
        </View>
      </View>
      <View style=styles##closeAction>
        <TouchableOpacity onPress=onPressCancel>
          <Icons.Close size=36. color=Theme.colorDarkMediumEmphasis />
        </TouchableOpacity>
      </View>
    </>
  </ModalView.Form>;
};
