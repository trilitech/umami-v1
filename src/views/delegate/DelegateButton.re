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
      "button":
        style(~alignSelf=`flexStart, ~overflow=`hidden, ~borderRadius=4., ()),
      "pressable":
        style(
          ~height=34.->dp,
          ~minWidth=104.->dp,
          ~paddingHorizontal=16.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~zeroTez, ~action: Delegate.action, ~style as styleFromProp=?) => {
  let theme = ThemeContext.useTheme();

  let tooltipId =
    "delegate_button_"
    ++ Delegate.account(action)->Option.mapWithDefault("", a => a.address);

  let (textColor, backgroundColor, text, tooltip) =
    switch (action) {
    | _ when zeroTez => (
        theme.colors.primaryTextDisabled,
        theme.colors.primaryButtonBackground,
        I18n.btn#delegate,
        Some((tooltipId, I18n.tooltip#no_tez_no_delegation)),
      )

    | Create(_) => (
        theme.colors.primaryTextHighEmphasis,
        theme.colors.primaryButtonBackground,
        I18n.btn#delegate,
        None,
      )
    | Edit(_)
    | Delete(_) => (
        theme.colors.primaryTextDisabled,
        theme.colors.iconDisabled,
        I18n.btn#delegated,
        Some((tooltipId, I18n.tooltip#update_delegation)),
      )
    };

  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _e => openAction();

  <>
    <View
      style=Style.(
        arrayOption([|
          Some(styles##button),
          Some(style(~backgroundColor, ())),
          styleFromProp,
        |])
      )>
      <ThemedPressable
        style=Style.(arrayOption([|Some(styles##pressable)|]))
        isPrimary=true
        ?tooltip
        disabled=zeroTez
        onPress
        accessibilityRole=`button>
        <Typography.ButtonPrimary style=Style.(style(~color=textColor, ()))>
          text->React.string
        </Typography.ButtonPrimary>
      </ThemedPressable>
    </View>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <DelegateView closeAction action />
    </ModalAction>
  </>;
};
