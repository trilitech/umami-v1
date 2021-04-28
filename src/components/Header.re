open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          ~height=60.->dp,
          ~alignItems=`center,
          ~paddingLeft=NavBar.width->dp,
          (),
        ),
      "nameLogo": style(~flexDirection=`row, ~alignItems=`center, ()),
      "networkInfo": style(~marginBottom=4.->dp, ~marginLeft=16.->dp, ()),
    })
  );

module UpdateNotice = {
  let styles =
    Style.(
      StyleSheet.create({
        "updateNotice":
          style(
            ~flexDirection=`row,
            ~height=60.->dp,
            ~alignItems=`center,
            ~paddingHorizontal=38.->dp,
            ~fontSize=14.,
            (),
          ),
        "upgradeButton":
          style(
            ~overflow=`hidden,
            ~borderRadius=4.,
            ~borderStyle=`solid,
            ~borderWidth=1.,
            (),
          ),
        "upgradePressable":
          style(
            ~height=26.->dp,
            ~minWidth=69.->dp,
            ~paddingHorizontal=8.->dp,
            ~justifyContent=`center,
            ~alignItems=`center,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~displayNotice) => {
    let theme = ThemeContext.useTheme();

    let onPress = _ =>
      System.openExternal(
        "https://gitlab.com/nomadic-labs/umami-wallet/umami/-/releases",
      );

    !displayNotice
      ? React.null
      : <View
          style=Style.(style(~backgroundColor=theme.colors.textPrimary, ()))>
          <View
            style=Style.(
              array([|
                style(
                  ~backgroundColor=theme.colors.backgroundMediumEmphasis,
                  (),
                ),
                styles##updateNotice,
              |])
            )>
            <Typography.Notice
              style=Style.(
                style(
                  ~color=theme.colors.textPrimary,
                  ~paddingRight=8.->dp,
                  (),
                )
              )>
              I18n.t#upgrade_notice->React.string
            </Typography.Notice>
            <View
              style=Style.(
                array([|
                  style(~borderColor=theme.colors.borderPrimary, ()),
                  styles##upgradeButton,
                |])
              )>
              <ThemedPressable
                style=Style.(
                  array([|
                    style(~color=theme.colors.textPrimary, ()),
                    styles##upgradePressable,
                  |])
                )
                isPrimary=true
                onPress
                accessibilityRole=`button>
                <Typography.ButtonPrimary
                  style=Style.(style(~color=theme.colors.textPrimary, ()))>
                  I18n.btn#upgrade->React.string
                </Typography.ButtonPrimary>
              </ThemedPressable>
            </View>
          </View>
        </View>;
  };
};

[@react.component]
let make = () => {
  let theme = ThemeContext.useTheme();
  let apiVersion = StoreContext.useApiVersion();

  let settings = SdkContext.useSettings();

  let displayNotice =
    apiVersion
    ->Option.map(apiVersion => !Network.checkInBound(apiVersion.Network.api))
    ->Option.getWithDefault(false);

  let (networkText, networkColor) =
    switch (settings->AppSettings.network) {
    | `Mainnet => (I18n.t#mainnet, Some(`primary))
    | `Testnet(_) => (I18n.t#testnet, None)
    };

  <View
    style=Style.(
      array([|
        styles##container,
        style(~backgroundColor=theme.colors.barBackground, ()),
      |])
    )>
    <View style={styles##nameLogo}>
      <SVGLogoUmami
        width={87.->Style.dp}
        height={20.->Style.dp}
        fill={theme.colors.textHighEmphasis}
      />
      <Typography.Overline2
        fontWeightStyle=`black
        colorStyle=?networkColor
        style={styles##networkInfo}>
        networkText->React.string
      </Typography.Overline2>
    </View>
    <UpdateNotice displayNotice />
  </View>;
};
