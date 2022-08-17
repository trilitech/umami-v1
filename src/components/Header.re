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

module NoticesView = {
  [@react.component]
  let make = (~notices) => {
    switch (notices) {
    | [] => React.null
    | [n, ..._] =>
      <NoticeView text={n.Notices.msg}>
        {n.Notices.btns
         ->List.toArray
         ->Array.mapWithIndex((i, btn) => {
             let style =
               if (i == 0) {
                 styles##noticeSpace;
               } else {
                 styles##buttonMargin;
               };
             <NoticeView.Button
               key={i->string_of_int}
               style
               onPress={_ => btn.Notices.onPress()}
               text={btn.Notices.text}
             />;
           })
         ->React.array}
      </NoticeView>
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

  let notices = NoticesContext.useNotices();
  let pushNotice = NoticesContext.usePush();
  let rmNotice = NoticesContext.useDelete();
  let apiVersion = StoreContext.useApiVersion();
  let retryNetwork = ConfigContext.useRetryNetwork();
  let networkOffline = ConfigContext.useNetworkOffline();
  React.useEffect0(_ => {
    let displayUpdateNotice =
      apiVersion
      ->Option.map(apiVersion =>
          !Network.checkInBound(apiVersion.Network.api)
        )
      ->Option.getWithDefault(false);
    if (displayUpdateNotice) {
      let btns =
        Notices.[
          {
            text: I18n.Btn.upgrade,
            onPress: _ => {
              System.openExternal(
                "https://gitlab.com/nomadic-labs/umami-wallet/umami/-/releases",
              );
            },
          },
        ];
      let notice =
        Notices.notice(~btns, ~key="upgrade_notice", I18n.upgrade_notice);
      pushNotice(notice);
    };
    None;
  });
  React.useEffect1(
    _ => {
      let key = "network_unreachable";
      let networkOffline_notice = {
        let btns =
          Notices.[
            {
              text: I18n.Btn.goto_settings,
              onPress: _ => {
                Routes.push(Settings);
              },
            },
            {
              text: I18n.Btn.retry_network,
              onPress: _ => {
                retryNetwork();
              },
            },
          ];
        Notices.notice(~btns, ~key, I18n.Errors.network_unreachable);
      };

      if (networkOffline) {
        pushNotice(networkOffline_notice);
      } else {
        rmNotice(key);
      };
      None;
    },
    [|networkOffline|],
  );
  React.useEffect0(_ => {
    let key = "download_complete";
    let btns =
      Notices.[
        {
          text: I18n.Btn.install_and_restart_now,
          onPress: _ => IPC.send("quit-and-install", ""),
        },
        {text: I18n.Btn.ill_do_it_later, onPress: _ => rmNotice(key)},
      ];
    let notice = Notices.notice(~btns, ~key, I18n.download_complete);
    IPC.on("update-downloaded", (_, _) => pushNotice(notice));
    None;
  });

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
    <NoticesView notices />
  </View>;
};
