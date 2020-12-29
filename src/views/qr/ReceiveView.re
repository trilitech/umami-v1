open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
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
  let token = StoreContext.SelectedToken.useGet();

  let theme = ThemeContext.useTheme();

  <ModalView.Form>
    <>
      <Typography.Headline style=FormStyles.header>
        account.alias->React.string
      </Typography.Headline>
      <AccountInfo account showAlias=false showBalance ?token />
      <View style=styles##qrContainer>
        <View style=styles##qr>
          <QRCode value={account.address} size=200. />
        </View>
      </View>
      <View style=styles##closeAction>
        <TouchableOpacity onPress=onPressCancel>
          <Icons.Close size=36. color={theme.colors.iconMediumEmphasis} />
        </TouchableOpacity>
      </View>
    </>
  </ModalView.Form>;
};
