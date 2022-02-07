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
        StyleSheet.flatten([|
          style(~borderRadius=35., ()),
          ShadowStyles.button,
        |]),
      "iconContainer":
        style(
          ~borderRadius=35.,
          ~width=70.->dp,
          ~height=70.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
      "textButton": style(~marginTop=6.->dp, ()),
    })
  );

[@react.component]
let make = (~account) => {
  let theme = ThemeContext.useTheme();

  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let onPress = _ => openAction();

  let networkOffline = ConfigContext.useNetworkOffline();

  let iconColor =
    networkOffline
      ? theme.colors.primaryIconDisabled : theme.colors.primaryIconHighEmphasis;

  let textColor =
    networkOffline
      ? theme.colors.primaryTextDisabled : theme.colors.primaryTextHighEmphasis;

  <>
    <View
      style=Style.(
        array([|
          styles##button,
          style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
        |])
      )>
      <ThemedPressable.Primary
        disabled=networkOffline
        style=styles##iconContainer
        onPress
        accessibilityRole=`button>
        <Icons.Send size=24. color=iconColor />
        <Typography.ButtonSecondary
          fontSize=13.
          style=Style.(
            array([|styles##textButton, style(~color=textColor, ())|])
          )>
          I18n.Btn.send->React.string
        </Typography.ButtonSecondary>
      </ThemedPressable.Primary>
    </View>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      <SendView account closeAction />
    </ModalAction>
  </>;
};
