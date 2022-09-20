/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2022 Nomadic Labs, <contact@nomadic-labs.com>          */
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

open ReactNative

let styles = {
  open Style
  StyleSheet.create({
    "itemContent": style(
      ~display=#flex,
      ~flexDirection=#row,
      ~alignItems=#center,
      ~paddingLeft=18.->dp,
      ~paddingVertical=10.->dp,
      ~width=100.->pct,
      (),
    ),
    "itemContentPage": style(
      ~display=#flex,
      ~flexDirection=#row,
      ~alignItems=#center,
      ~paddingLeft=0.->dp,
      ~width=100.->pct,
      (),
    ),
    "logBlock": style(~paddingVertical=12.->dp, ~marginHorizontal=8.->dp, ()),
    "actionButtons": style(~flexDirection=#row, ~flexShrink=0., ~marginLeft=auto, ()),
    "reqelt": style(~flexShrink=0., ~marginRight=16.->dp, ()),
    "kindIcon": style(~marginRight=10.->dp, ()),
    "container": style(~borderRadius=8., ~marginTop=10.->dp, ()),
    "primaryText": style(~color=Colors.Dark.primary, ()),
    "item": style(
      ~display=#flex,
      ~flexDirection=#row,
      ~borderRadius=8.,
      ~flexWrap=#nowrap,
      ~width=100.->pct,
      ~alignSelf=#center,
      ~justifyContent=#flexStart,
      (),
    ),
    "button": style(~marginRight=4.->dp, ()),
    "toggleOff": style(~paddingLeft=32.->dp, ~width=100.->pct, ~paddingTop=0.->dp, ()),
    "reverseY": style(~transform=[rotate(~rotate=180.->deg)], ()),
    "firstLine": style(~marginRight=49.->dp, ()),
  })
}

module DeleteButton = {
  @react.component
  let make = (~isPrimary=?, ~style=?, ~toast, ~indice, ~handleDelete) => {
    let onPress = _ => handleDelete(indice)
    <IconButton ?isPrimary ?style toast icon=Icons.Delete.build onPress />
  }
}

module CloseButton = {
  @react.component
  let make = (~isPrimary=?, ~style=?, ~toast, ~indice, ~handleDelete) => {
    let onPress = _ => handleDelete(indice)
    <IconButton ?isPrimary ?style toast icon=Icons.Close.build onPress />
  }
}

module OpenButton = {
  @react.component
  let make = (~isPrimary=?, ~opened, ~setOpened, ~chevronStyle) => {
    let onPress = _ => setOpened(_ => !opened)
    <IconButton ?isPrimary style=chevronStyle icon=Icons.ChevronDown.build onPress />
  }
}

let buildScopeText = (log: Logs.t) => log.errorScope->Option.mapWithDefault("", sc => sc ++ "/")

let buildMessage = (log: Logs.t) => {
  let scTxt = buildScopeText(log)
  Format.sprintf("(%s%s): %s", scTxt, log.origin->Logs.originToString, log.msg)
}

let actionButtons = (~indice, ~log: Logs.t, ~addToast, ~handleDelete, ~toast) =>
  <View style={styles["actionButtons"]}>
    {log.kind == Error
      ? <MailToSupportButton toast message={buildMessage(log)} style={styles["button"]} />
      : React.null}
    {<ClipboardButton
      isPrimary=false
      data=log.msg
      toast
      copied=I18n.Log.log_content
      addToast
      style={styles["button"]}
    />->ReactUtils.onlyWhen(log.kind == Error)}
    {toast
      ? <CloseButton isPrimary=false indice handleDelete toast style={styles["button"]} />
      : <DeleteButton isPrimary=false indice handleDelete toast style={styles["button"]} />}
  </View>

module Entry = {
  @react.component
  let make = (~isFirst=false, ~log: Logs.t, ~addToast, ~indice, ~handleDelete) => {
    let theme = ThemeContext.useTheme()

    let (opened, setOpened) = React.useState(_ => false)

    let chevronStyle = styleProp =>
      opened
        ? {
            open Style
            array([styleProp, styles["reverseY"]])
          }
        : styleProp

    let logDateContent = {
      open Js.Date
      log.timestamp->fromFloat->toUTCString
    }->React.string

    let logsBackgroundColor = opened ? theme.colors.stateActive : theme.colors.background

    let firstline = opened
      ? React.null
      : <Typography.InPageLog
          ellipsizeMode=#tail
          style={
            open Style
            array([styles["firstLine"], style(~color=theme.colors.textMaxEmphasis, ())])
          }
          fontWeightStyle=#light
          numberOfLines=1
          content={buildMessage(log)->React.string}
        />

    let secondline = opened
      ? <Typography.InPageLog
          fontWeightStyle=#light
          style={
            open Style
            array([
              styles["itemContentPage"],
              styles["toggleOff"],
              style(~color=theme.colors.textMaxEmphasis, ()),
            ])
          }
          content={buildMessage(log)->React.string}
        />
      : React.null

    let onPress = _ => setOpened(_ => !opened)

    <TouchableOpacity
      onPress
      style={
        open Style
        array([
          styles["logBlock"],
          isFirst
            ? style()
            : style(~borderTopColor=theme.colors.stateDisabled, ~borderTopWidth=1., ()),
          style(~backgroundColor=logsBackgroundColor, ()),
        ])
      }>
      <View>
        <View style={styles["itemContentPage"]}>
          <OpenButton
            isPrimary=false opened setOpened chevronStyle={chevronStyle(styles["button"])}
          />
          <Typography.InPageLog
            style={
              open Style
              array([styles["reqelt"], style(~color=theme.colors.textMaxEmphasis, ())])
            }
            numberOfLines=1
            fontWeightStyle=#bold
            content=logDateContent
          />
          firstline
          {actionButtons(~indice, ~log, ~addToast, ~handleDelete, ~toast=false)}
        </View>
        secondline
      </View>
    </TouchableOpacity>
  }
}

module Toast = {
  @react.component
  let make = (~log: Logs.t, ~addToast, ~indice, ~handleDelete) => {
    let theme = ThemeContext.useTheme()

    let icon = switch log.kind {
    | Error
    | Warning =>
      <Icons.Error size=20. color=theme.colors.toastError />
    | Info => <Icons.CheckFill size=20. color=theme.colors.toastValid />
    }

    let jsx_of_btn = (btn: Logs.btn) =>
      <ButtonAction text=btn.text onPress={_ => btn.onPress()} primary=true />

    let buttons =
      log.btns->Option.mapWithDefault(React.null, btns =>
        btns->List.map(jsx_of_btn)->List.toArray->React.array
      )

    <View
      style={
        open Style
        array([styles["container"], style(~backgroundColor=theme.colors.logBackground, ())])
      }>
      <Hoverable
        style={
          open Style
          array([styles["item"]])
        }
        hoveredStyle={Style.style(~backgroundColor=theme.colors.primaryStateHovered, ())}>
        {_ =>
          <View style={styles["itemContent"]}>
            <View style={styles["kindIcon"]}> icon </View>
            <Typography.Body1
              style={Style.style(~color=theme.colors.primaryTextMediumEmphasis, ())}
              fontWeightStyle=#bold
              ellipsizeMode=#tail
              numberOfLines=1>
              {log.msg->React.string}
            </Typography.Body1>
            buttons
            {actionButtons(~indice, ~log, ~addToast, ~handleDelete, ~toast=true)}
          </View>}
      </Hoverable>
    </View>
  }
}
