/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNative

let styles = {
  open Style
  StyleSheet.create({
    "notice": style(
      ~flexDirection=#row,
      ~height=60.->dp,
      ~alignItems=#center,
      ~paddingHorizontal=38.->dp,
      ~fontSize=14.,
      (),
    ),
    "noticeSpace": style(~marginLeft=24.->dp, ()),
    "buttonMargin": style(~marginLeft=10.->dp, ()),
    "button": style(~overflow=#hidden, ~borderRadius=4., ~borderStyle=#solid, ~borderWidth=1., ()),
    "pressable": style(
      ~height=26.->dp,
      ~minWidth=69.->dp,
      ~paddingHorizontal=8.->dp,
      ~justifyContent=#center,
      ~alignItems=#center,
      (),
    ),
  })
}

module Button = {
  @react.component
  let make = (~style as styleArg=?, ~text, ~onPress) => {
    let theme = ThemeContext.useTheme()
    <View
      style={
        open Style
        arrayOption([
          style(~borderColor=theme.colors.borderPrimary, ())->Some,
          styles["button"]->Some,
          styleArg,
        ])
      }>
      <ThemedPressable.Primary
        style={
          open Style
          array([style(~color=theme.colors.textPrimary, ()), styles["pressable"]])
        }
        onPress
        accessibilityRole=#button>
        <Typography.ButtonPrimary
          style={
            open Style
            style(~color=theme.colors.textPrimary, ())
          }>
          {text->React.string}
        </Typography.ButtonPrimary>
      </ThemedPressable.Primary>
    </View>
  }
}

type btn = {
  text: string,
  onPress: unit => unit,
}

type t = {
  msg: string,
  btns: list<btn>,
}

module NoticeView = {
  @react.component
  let make = (~style as styleArg=?, ~children, ~text) => {
    let theme = ThemeContext.useTheme()

    <View
      style={
        open Style
        arrayOption([style(~backgroundColor=theme.colors.textPrimary, ())->Some, styleArg])
      }>
      <View
        style={
          open Style
          array([
            style(~backgroundColor=theme.colors.backgroundMediumEmphasis, ()),
            styles["notice"],
          ])
        }>
        <Typography.Notice
          style={
            open Style
            style(~color=theme.colors.textPrimary, ~paddingRight=8.->dp, ())
          }>
          {text->React.string}
        </Typography.Notice>
        children
      </View>
    </View>
  }
}

@react.component
let make = (~style=?, ~notice) => {
  let rmNotice = NoticesContext.useDelete()

  let text = switch notice {
  | NoticesContext.Notice_network_unreachable => I18n.Errors.network_unreachable
  | NoticesContext.Notice_update_downloaded => I18n.download_complete
  | NoticesContext.Notice_update_required => I18n.upgrade_notice
  }

  let btns = switch notice {
  | NoticesContext.Notice_network_unreachable => [
      {
        text: I18n.Btn.goto_settings,
        onPress: _ => Routes.push(Settings),
      },
      {
        text: I18n.Btn.retry_network,
        onPress: _ => ConfigContext.useRetryNetwork()(),
      },
    ]
  | NoticesContext.Notice_update_required => [
      {
        text: I18n.Btn.upgrade,
        onPress: _ =>
          System.openExternal("https://github.com/trilitech/umami-v1/releases"),
      },
    ]
  | NoticesContext.Notice_update_downloaded => [
      {
        text: I18n.Btn.install_and_restart_now,
        onPress: _ => IPC.send("quit-and-install", ""),
      },
      {text: I18n.Btn.ill_do_it_later, onPress: _ => rmNotice(notice)},
    ]
  }
  <NoticeView ?style text>
    {btns
    ->Array.mapWithIndex((i, btn) => {
      let style = if i == 0 {
        styles["noticeSpace"]
      } else {
        styles["buttonMargin"]
      }
      <Button key={i->string_of_int} style onPress={_ => btn.onPress()} text=btn.text />
    })
    ->React.array}
  </NoticeView>
}
