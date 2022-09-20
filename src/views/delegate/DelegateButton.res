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
    "button": style(~alignSelf=#flexStart, ~borderRadius=4., ()),
    "pressable": style(
      ~borderRadius=4.,
      ~height=34.->dp,
      ~minWidth=104.->dp,
      ~paddingHorizontal=16.->dp,
      ~justifyContent=#center,
      ~alignItems=#center,
      (),
    ),
  })
}

@react.component
let make = (~zeroTez, ~action: Delegate.action, ~style as styleFromProp=?) => {
  let theme = ThemeContext.useTheme()

  let tooltipId =
    "delegate_button_" ++
    Delegate.account(action)->Option.mapWithDefault("", a => (a.address :> string))

  let (textColor, backgroundColor, focusOutlineColor, text, tooltip) = switch action {
  | _ if zeroTez => (
      theme.colors.primaryTextDisabled,
      theme.colors.primaryButtonBackground,
      None,
      I18n.Btn.delegate,
      Some((tooltipId, I18n.Tooltip.no_tez_no_delegation)),
    )
  | Create(_) => (
      theme.colors.primaryTextHighEmphasis,
      theme.colors.primaryButtonBackground,
      None,
      I18n.Btn.delegate,
      None,
    )
  | Edit(_)
  | Delete(_) => (
      theme.colors.primaryTextDisabled,
      theme.colors.surfaceButtonBackground,
      Some(theme.colors.surfaceButtonOutline),
      I18n.Btn.delegated,
      Some((tooltipId, I18n.Tooltip.update_delegation)),
    )
  }

  let (visibleModal, openAction, closeAction) = ModalAction.useModalActionState()

  let onPress = _e => openAction()

  let pressableElement = (~pressableRef) =>
    <ThemedPressable.Primary
      ?pressableRef
      style={
        open Style
        arrayOption([Some(styles["pressable"])])
      }
      focusedColor=?focusOutlineColor
      disabled=zeroTez
      onPress
      accessibilityRole=#button>
      <Typography.ButtonPrimary
        style={
          open Style
          style(~color=textColor, ())
        }>
        {text->React.string}
      </Typography.ButtonPrimary>
    </ThemedPressable.Primary>

  <>
    <View
      style={
        open Style
        arrayOption([Some(styles["button"]), Some(style(~backgroundColor, ())), styleFromProp])
      }>
      {switch tooltip {
      | Some((keyPopover, text)) =>
        <Tooltip keyPopover text>
          {(~pressableRef) => pressableElement(~pressableRef=Some(pressableRef))}
        </Tooltip>
      | None => pressableElement(~pressableRef=None)
      }}
    </View>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <DelegateView closeAction action />
    </ModalAction>
  </>
}
