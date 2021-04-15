open ReactNative;

%raw
"var Electron = window.require('electron');";
let electron = [%raw "Electron"];

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

    let onPress = _ => {
      electron##shell##openExternal(
        "https://gitlab.com/nomadic-labs/umami-wallet/umami/-/releases",
      )
      ->ignore;
    };

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

  let displayNotice =
    apiVersion
    ->Option.map(apiVersion => !Network.checkInBound(apiVersion.Network.api))
    ->Option.getWithDefault(false);

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
    </View>
    <UpdateNotice displayNotice />
  </View>;
};
