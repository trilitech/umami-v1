/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

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
      "buttonMargin": style(~marginLeft=10.->dp, ()),
      "noticeSpace": style(~marginLeft=24.->dp, ()),
      "networkHeader":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~marginLeft=16.->dp,
          ~marginBottom=2.->dp,
          (),
        ),
      "tag":
        style(
          ~height=26.->dp,
          ~paddingHorizontal=16.->dp,
          ~paddingVertical=4.->dp,
          ~marginRight=8.->dp,
          ~borderRadius=13.,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderWidth=1.,
          (),
        ),
    })
  );

module Notice = {
  let styles =
    Style.(
      StyleSheet.create({
        "notice":
          style(
            ~flexDirection=`row,
            ~height=60.->dp,
            ~alignItems=`center,
            ~paddingHorizontal=38.->dp,
            ~fontSize=14.,
            (),
          ),
        "button":
          style(
            ~overflow=`hidden,
            ~borderRadius=4.,
            ~borderStyle=`solid,
            ~borderWidth=1.,
            (),
          ),
        "pressable":
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

  module Button = {
    [@react.component]
    let make = (~style as styleArg=?, ~text, ~onPress) => {
      let theme = ThemeContext.useTheme();
      <View
        style=Style.(
          arrayOption([|
            style(~borderColor=theme.colors.borderPrimary, ())->Some,
            styles##button->Some,
            styleArg,
          |])
        )>
        <ThemedPressable.Primary
          style=Style.(
            array([|
              style(~color=theme.colors.textPrimary, ()),
              styles##pressable,
            |])
          )
          onPress
          accessibilityRole=`button>
          <Typography.ButtonPrimary
            style=Style.(style(~color=theme.colors.textPrimary, ()))>
            text->React.string
          </Typography.ButtonPrimary>
        </ThemedPressable.Primary>
      </View>;
    };
  };

  [@react.component]
  let make = (~style as styleArg=?, ~children, ~text) => {
    let theme = ThemeContext.useTheme();

    <View
      style=Style.(
        arrayOption([|
          style(~backgroundColor=theme.colors.textPrimary, ())->Some,
          styleArg,
        |])
      )>
      <View
        style=Style.(
          array([|
            style(~backgroundColor=theme.colors.backgroundMediumEmphasis, ()),
            styles##notice,
          |])
        )>
        <Typography.Notice
          style=Style.(
            style(~color=theme.colors.textPrimary, ~paddingRight=8.->dp, ())
          )>
          text->React.string
        </Typography.Notice>
        children
      </View>
    </View>;
  };
};

module Notices = {
  [@react.component]
  let make = () => {
    let apiVersion = StoreContext.useApiVersion();
    let retryNetwork = ConfigContext.useRetryNetwork();

    let displayUpdateNotice =
      apiVersion
      ->Option.map(apiVersion =>
          !Network.checkInBound(apiVersion.Network.api)
        )
      ->Option.getWithDefault(false);

    let networkOffline = ConfigContext.useNetworkOffline();

    if (displayUpdateNotice) {
      let onPress = _ =>
        System.openExternal(
          "https://gitlab.com/nomadic-labs/umami-wallet/umami/-/releases",
        );
      <Notice text=I18n.t#upgrade_notice>
        <Notice.Button onPress text=I18n.btn#upgrade />
      </Notice>;
    } else if (networkOffline) {
      <Notice text=I18n.errors#network_unreachable>
        <Notice.Button
          style=styles##noticeSpace
          onPress={_ => Routes.push(Settings)}
          text=I18n.btn#goto_settings
        />
        <Notice.Button
          style=styles##buttonMargin
          onPress={_ => {retryNetwork()}}
          text=I18n.btn#retry_network
        />
      </Notice>;
    } else {
      React.null;
    };
  };
};

[@react.component]
let make = () => {
  let theme = ThemeContext.useTheme();

  let config = ConfigContext.useContent();

  let (networkText, networkColor) = (
    config.network.name,
    config.network.chain == `Mainnet
      ? Some(`primary) : Some(`mediumEmphasis),
  );

  let tag = {
    config.defaultNetwork
      ? None : config.network.chain->Network.getDisplayedName->Some;
  };

  let tagBorderColor = {
    config.network.chain == `Mainnet
      ? theme.colors.textPrimary : theme.colors.textMediumEmphasis;
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
      <View style=styles##networkHeader>
        {<View
           style=Style.(
             array([|styles##tag, style(~borderColor=tagBorderColor, ())|])
           )>
           <Typography.Body2
             fontSize=14. colorStyle={Option.getExn(networkColor)}>
             {switch (tag) {
              | Some(content) => "Custom " ++ content
              | None => ""
              }}
             ->React.string
           </Typography.Body2>
         </View>
         ->ReactUtils.onlyWhen(tag != None)}
        <Typography.Overline2 fontWeightStyle=`black colorStyle=?networkColor>
          networkText->React.string
        </Typography.Overline2>
      </View>
    </View>
    <Notices />
  </View>;
};
