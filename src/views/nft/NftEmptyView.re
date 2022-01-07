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
open Style;

let paddingTop = 100.->dp;

let iconStyle =
  style(~paddingTop, ~paddingLeft=50.->dp, ~flex=1., ~maxWidth=370.->dp, ());

let textStyle = style(~textAlign=`left, ~paddingBottom=30.->dp, ());

[@react.component]
let make = () => {
  let theme = ThemeContext.useTheme();

  <View
    style={style(
      ~display=`flex,
      ~flexDirection=`row,
      ~justifyContent=`flexStart,
      ~alignItems=`center,
      (),
    )}>
    <Icons.Nft.I size=272. color={theme.colors.textDisabled} style=iconStyle />
    <View style={style(~flex=1., ~paddingTop=150.->dp, ())}>
      <Typography.BigText style=textStyle>
        I18n.you_dont_have_nft->React.string
      </Typography.BigText>
      <Typography.MediumText style=textStyle>
        I18n.Expl.nft_empty_state->React.string
      </Typography.MediumText>
    </View>
  </View>;
};
